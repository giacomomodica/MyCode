# you might need to install the vagrant-vbguest plugin
# vagrant plugin install vagrant-vbguest

# provision script
#-----------------------------------------------------------------------
$bootstrap = <<BOOTSTRAP
export DEBIAN_FRONTEND=noninteractive
apt-get update && apt-get upgrade

# install base development tools
apt-get -y install build-essential
apt-get -y install cmake valgrind lcov
apt-get -y install python3-pip
pip3 install gcovr
pip3 install pexpect

# install lightweight graphical environment
apt-get -y install xorg sddm openbox midori

echo "[Autologin]" >> /etc/sddm.conf
echo "User=vagrant" >> /etc/sddm.conf
echo "Session=openbox.desktop" >> /etc/sddm.conf

# install Qt dev libs
apt-get -y install qt5-default
BOOTSTRAP
#-----------------------------------------------------------------------

Vagrant.configure("2") do |config|
  config.vm.box = "ubuntu/xenial64"

  # get rig of annoying console.log file 
  config.vm.provider "virtualbox" do |vb|
    vb.customize [ "modifyvm", :id, "--uartmode1", "disconnected" ]
    vb.gui = true
  end

  # setup the VM
  config.vm.provision "shell", inline: $bootstrap
end
