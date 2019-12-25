g!/bin/sh

cd Assets/Resources
mkdir txt
rm txt/*.txt
rm *.bytes
for ((i=0;i<10;i++))
do
  cat /dev/urandom | base64 | fold -w 64 | head -n 1000 > txt/sample$i.txt
done
zstd txt/sample0.txt -o sample_deflated.bytes
zstd --train -r txt -o sample_dict.bytes
zstd -D sample_dict.bytes txt/sample0.txt -o sample_dictdeflated.bytes


