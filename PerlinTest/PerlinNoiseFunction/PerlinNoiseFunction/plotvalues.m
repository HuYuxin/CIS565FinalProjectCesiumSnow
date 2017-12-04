fid = fopen('PerlinTest.txt');
values = textscan(fid, '%f');
fclose(fid);
figure;
plot(values{1})