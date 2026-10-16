psnr: psnr.cc ssim.cc psnr_main.cc ../libyuv.a
ifeq ($(CXX),icl)
	$(CXX) /arch:SSE2 /Ox /openmp /I../include psnr.cc ssim.cc psnr_main.cc ../libyuv.a
else
	$(CXX) -msse2 -O3 -fopenmp -I../include -o psnr psnr.cc ssim.cc psnr_main.cc ../libyuv.a -Wl,--strip-all
endif

# for MacOS
# /usr/local/bin/g++-7 -msse2 -O3 -fopenmp -Bstatic -I../include -o psnr psnr.cc ssim.cc psnr_main.cc ../libyuv.a
