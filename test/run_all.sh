pwd=$PWD
source ~/tools/designplayer-shell/10APR2018/setup_env.sh

declare -a arr1=("b01" "b06" "b10" "b11" "b14" "or1200_ICache" "or1200_DCache" "or1200_Exception" "PCI" "usb_phy" "memctrl-T100")

for d in "${arr1[@]}"
#for d in $(find ./ -maxdepth 1 -type d)
do
	cd $pwd/$d

	make | tee dump &
done

pwd=$pwd/AES
cd $pwd

declare -a arr2=("AES-T1100" "AES-T2000")
for d in "${arr2[@]}"
#for d in $(find ./ -maxdepth 1 -type d)
do
	cd $pwd/$d
	make | tee dump &
done


