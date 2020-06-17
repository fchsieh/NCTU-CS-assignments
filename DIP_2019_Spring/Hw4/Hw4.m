clc;
clear all;
close all;

filename = "HW4_einstein.tif";
img = imread(filename);

% ----------- Part 1
% DFT
% 1. magnitude
f1 = fft2(img);
mag1 = abs(f1);
mag1 = real(log(1 + mag1)); % display in log scale
imwrite(Normalize(mag1), "part_1_magnitude.png");
% 2. phase
phase1 = angle(f1);
imwrite(Normalize(phase1), "part_1_phase.png");

% ----------- Part 2
% DFT
% 1. magnitude
f2 = fft2(img);
mag2 = fftshift(abs(f2)); % shifting fft result
mag2 = real(log(1 + mag2));
imwrite(Normalize(mag2), "part_2_magnitude.png");
% 2. phase
phase2 = angle(fftshift(f2));
imshow(unwrap(phase2, []);)
imwrite(Normalize(phase2), "pa)rt_2_phase.png");

% ----------- Part 3.1
f3 = fft2(img);
% form gaussian filter function
sz = size(img);
h = fspecial("gaussian", sz, 10); % set sigma = 10
H = fft2(h);
G = f3 .* H; % perform filtering in the frequency domain
g = real(ifftshift(ifft2(G))); % shift to the center
imwrite(Normalize(g), "part_3_sigma=10.png");

% ----------- Try different parameters(sigma = 50)
h = fspecial("gaussian", sz, 50); % set sigma = 50
H = fft2(h);
G = f3 .* H; % perform filtering in the frequency domain
g = real(ifftshift(ifft2(G))); % shift to the center
imwrite(Normalize(g), "part_3_sigma=50.png");

% ----------- Part 3.2
% Step 1. set p = 2m, q = 2n
[m, n] = size(img);
p = 2 * m;
q = 2 * n;
% Step 2. form padded image fp of size p*q using zero padding
pad_img = padarray(img, sz, "post");
% Step 3. Multiply fp by (-1)^(x + y)
fp = zeros(p, q);
for i = 1:p
    for j = 1:q
        fp(i, j) = pad_img(i, j) .* ((- 1) .^ (i + j));
    end
end
% Step 4. Compute DFT of fp
F = fftshift(fft2(fp));
% Step 5. Construct gaussian filter
h = fspecial("gaussian", [p, q], 10); % sigma = 10
H = fftshift(fft2(h));
% Step 6. Form G = H * F
G = H .* F;
% Step 7. get gp
g = real(ifftshift(ifft2(G)));
gp = zeros(p, q);
for i = 1:p
    for j = 1:q
        gp(i, j) = g(i, j) .* ((- 1) .^ (i + j));
    end
end
% Step 8. get g by extractin top, left quadrant
out_g = zeros(m, n);
for i = 1:m
    for j = 1:n
        out_g(i, j) = gp(i, j);
    end
end
imwrite(Normalize(out_g), "part_3_with_padding.png");

function normalized_img = Normalize(img)
    min_of_img = min(img(:));
    factor = (255 - 0) / (max(img(:)) - min_of_img);
    normalized_img = uint8((img - min_of_img) .* factor);
end