define([
        '../Core/BoundingRectangle',
        '../Core/Color',
        '../Core/defined',
        '../Core/destroyObject',
        '../Core/PixelFormat',
        '../Renderer/ClearCommand',
        '../Renderer/Framebuffer',
        '../Renderer/PixelDatatype',
        '../Renderer/RenderState',
        '../Renderer/Sampler',
        '../Renderer/Texture',
        '../Renderer/TextureWrap',
        '../Renderer/TextureMagnificationFilter',
        '../Renderer/TextureMinificationFilter',
        '../shaders/PostProcessFilters/DepthViewPacked',
        '../Shaders/PostProcessFilters/PassThrough',
        '../Shaders/PostProcessFilters/PassThroughDepth'
    ], function(
        BoundingRectangle,
        Color,
        defined,
        destroyObject,
        PixelFormat,
        ClearCommand,
        Framebuffer,
        PixelDatatype,
        RenderState,
        Sampler,
        Texture,
        TextureWrap,
        TextureMagnificationFilter,
        TextureMinificationFilter,
        DepthViewPacked,
        PassThrough,
        PassThroughDepth) {
    'use strict';

    /**
     * @private
     */
    function GlobeDepth() {
        this._colorTexture = undefined;
        this._depthStencilTexture = undefined;
        this._globeDepthTexture = undefined;

        this.framebuffer = undefined;
        this._copyDepthFramebuffer = undefined;

        this._clearColorCommand = undefined;
        this._copyColorCommand = undefined;
        this._copyDepthCommand = undefined;

        this._viewport = new BoundingRectangle();
        this._rs = undefined;

        this._useScissorTest = false;
        this._scissorRectangle = undefined;

        this._debugGlobeDepthViewportCommand = undefined;
    }

    function executeDebugGlobeDepth(globeDepth, context, passState) {
        if (!defined(globeDepth._debugGlobeDepthViewportCommand)) {
            globeDepth._debugGlobeDepthViewportCommand = context.createViewportQuadCommand(DepthViewPacked, {
                uniformMap : {
                    u_depthTexture : function() {
                        return globeDepth._globeDepthTexture;
                    }
                },
                owner : globeDepth
            });
        }

        globeDepth._debugGlobeDepthViewportCommand.execute(context, passState);
    }

    function destroyTextures(globeDepth) {
        globeDepth._colorTexture = globeDepth._colorTexture && !globeDepth._colorTexture.isDestroyed() && globeDepth._colorTexture.destroy();
        globeDepth._depthStencilTexture = globeDepth._depthStencilTexture && !globeDepth._depthStencilTexture.isDestroyed() && globeDepth._depthStencilTexture.destroy();
        globeDepth._globeDepthTexture = globeDepth._globeDepthTexture && !globeDepth._globeDepthTexture.isDestroyed() && globeDepth._globeDepthTexture.destroy();
    }

    function destroyFramebuffers(globeDepth) {
        globeDepth.framebuffer = globeDepth.framebuffer && !globeDepth.framebuffer.isDestroyed() && globeDepth.framebuffer.destroy();
        globeDepth._copyDepthFramebuffer = globeDepth._copyDepthFramebuffer && !globeDepth._copyDepthFramebuffer.isDestroyed() && globeDepth._copyDepthFramebuffer.destroy();
    }

    function createTextures(globeDepth, context, width, height) {
        globeDepth._colorTexture = new Texture({
            context : context,
            width : width,
            height : height,
            pixelFormat : PixelFormat.RGBA,
            pixelDatatype : PixelDatatype.UNSIGNED_BYTE,
            sampler : new Sampler({
                wrapS : TextureWrap.CLAMP_TO_EDGE,
                wrapT : TextureWrap.CLAMP_TO_EDGE,
                minificationFilter : TextureMinificationFilter.NEAREST,
                magnificationFilter : TextureMagnificationFilter.NEAREST
            })
        });

        globeDepth._depthStencilTexture = new Texture({
            context : context,
            width : width,
            height : height,
            pixelFormat : PixelFormat.DEPTH_STENCIL,
            pixelDatatype : PixelDatatype.UNSIGNED_INT_24_8
        });

        globeDepth._globeDepthTexture = new Texture({
            context : context,
            width : width,
            height : height,
            pixelFormat : PixelFormat.RGBA,
            pixelDatatype : PixelDatatype.UNSIGNED_BYTE
        });
    }

    function createFramebuffers(globeDepth, context) {
        globeDepth.framebuffer = new Framebuffer({
            context : context,
            colorTextures : [globeDepth._colorTexture],
            depthStencilTexture : globeDepth._depthStencilTexture,
            destroyAttachments : false
        });

        globeDepth._copyDepthFramebuffer = new Framebuffer({
            context : context,
            colorTextures : [globeDepth._globeDepthTexture],
            destroyAttachments : false
        });
    }

    function updateFramebuffers(globeDepth, context, width, height) {
        var colorTexture = globeDepth._colorTexture;
        var textureChanged = !defined(colorTexture) || colorTexture.width !== width || colorTexture.height !== height;
        if (!defined(globeDepth.framebuffer) || textureChanged) {
            destroyTextures(globeDepth);
            destroyFramebuffers(globeDepth);
            createTextures(globeDepth, context, width, height);
            createFramebuffers(globeDepth, context);
        }
    }

    function updateCopyCommands(globeDepth, context, width, height, passState) {
        globeDepth._viewport.width = width;
        globeDepth._viewport.height = height;

        var useScissorTest = !BoundingRectangle.equals(globeDepth._viewport, passState.viewport);
        var updateScissor = useScissorTest !== globeDepth._useScissorTest;
        globeDepth._useScissorTest = useScissorTest;

        if (!BoundingRectangle.equals(globeDepth._scissorRectangle, passState.viewport)) {
            globeDepth._scissorRectangle = BoundingRectangle.clone(passState.viewport, globeDepth._scissorRectangle);
            updateScissor = true;
        }

        if (!defined(globeDepth._rs) || !BoundingRectangle.equals(globeDepth._viewport, globeDepth._rs.viewport) || updateScissor) {
            globeDepth._rs = RenderState.fromCache({
                viewport : globeDepth._viewport,
                scissorTest : {
                    enabled : globeDepth._useScissorTest,
                    rectangle : globeDepth._scissorRectangle
                }
            });
        }

        if (!defined(globeDepth._copyDepthCommand)) {
            globeDepth._copyDepthCommand = context.createViewportQuadCommand(PassThroughDepth, {
                uniformMap : {
                    u_depthTexture : function() {
                        return globeDepth._depthStencilTexture;
                    }
                },
                owner : globeDepth
            });
        }

        globeDepth._copyDepthCommand.framebuffer = globeDepth._copyDepthFramebuffer;

        if (!defined(globeDepth._copyColorCommand)) {
            globeDepth._copyColorCommand = context.createViewportQuadCommand(PassThrough, {
                uniformMap : {
                    u_colorTexture : function() {
                        return globeDepth._colorTexture;
                    }
                },
                owner : globeDepth
            });
        }

        globeDepth._copyDepthCommand.renderState = globeDepth._rs;
        globeDepth._copyColorCommand.renderState = globeDepth._rs;

        if (!defined(globeDepth._clearColorCommand)) {
            globeDepth._clearColorCommand = new ClearCommand({
                color : new Color(0.0, 0.0, 0.0, 0.0),
                stencil : 0.0,
                owner : globeDepth
            });
        }

        globeDepth._clearColorCommand.framebuffer = globeDepth.framebuffer;
    }

    GlobeDepth.prototype.executeDebugGlobeDepth = function(context, passState) {
        executeDebugGlobeDepth(this, context, passState);
    };

    GlobeDepth.prototype.update = function(context, passState) {
        var width = context.drawingBufferWidth;
        var height = context.drawingBufferHeight;

        updateFramebuffers(this, context, width, height);
        updateCopyCommands(this, context, width, height, passState);
        context.uniformState.globeDepthTexture = undefined;
    };

    GlobeDepth.prototype.executeCopyDepth = function(context, passState) {
        if (defined(this._copyDepthCommand)) {
            this._copyDepthCommand.execute(context, passState);
            context.uniformState.globeDepthTexture = this._globeDepthTexture;
        }
    };

    GlobeDepth.prototype.executeCopyColor = function(context, passState) {
        if (defined(this._copyColorCommand)) {
            this._copyColorCommand.execute(context, passState);
        }
    };

    GlobeDepth.prototype.clear = function(context, passState, clearColor) {
        var clear = this._clearColorCommand;
        if (defined(clear)) {
            Color.clone(clearColor, clear.color);
            clear.execute(context, passState);
        }
    };

    GlobeDepth.prototype.isDestroyed = function() {
        return false;
    };

    GlobeDepth.prototype.destroy = function() {
        destroyTextures(this);
        destroyFramebuffers(this);

        if (defined(this._copyColorCommand)) {
            this._copyColorCommand.shaderProgram = this._copyColorCommand.shaderProgram.destroy();
        }

        if (defined(this._copyDepthCommand)) {
            this._copyDepthCommand.shaderProgram = this._copyDepthCommand.shaderProgram.destroy();
        }

        var command = this._debugGlobeDepthViewportCommand;
        if (defined(command)) {
            command.shaderProgram = command.shaderProgram.destroy();
        }

        return destroyObject(this);
    };

    return GlobeDepth;
});
