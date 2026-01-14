# install Pythia
wget https://pythia.org/download/pythia83/pythia8309.tgz
tar -xzf pythia8309.tgz
cd pythia8309
./configure --prefix=$CONDA_PREFIX
make -j4
make install
