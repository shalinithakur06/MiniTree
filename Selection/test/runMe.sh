inNtupleFile=$1
outAnalFile=$2

echo "INPUT FILE: " $inNtupleFile
cp electronNtuple_cfg.py ../ 
sed -i "s:inFile_:$inNtupleFile:g" electronNtuple_cfg.py
sed -i "s:outFile_:$outAnalFile:g" electronNtuple_cfg.py
cmsRun electronNtuple_cfg.py
mv ../electronNtuple_cfg.py .
