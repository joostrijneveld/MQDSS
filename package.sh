mkdir mqsig
mkdir mqsig/scripts
cp scripts/rectanglerotates.py mqsig/scripts/rectanglerotates.py
cp -R 3pass_sign mqsig
cp -R 5pass_sign mqsig
cp -R common mqsig
cp README.md mqsig
make -C mqsig/3pass_sign clean > /dev/null
make -C mqsig/5pass_sign clean > /dev/null
COPYFILE_DISABLE=1 tar -hczv --exclude='.gitignore' mqsig
rm -rf mqsig
