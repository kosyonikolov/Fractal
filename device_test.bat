adb push build/RsaFractal /data/local/tmp
adb shell "cd /data/local/tmp; rm fractal2.bmp; chmod u+x RsaFractal; ./RsaFractal"

adb pull /data/local/tmp/fractal2.bmp test/