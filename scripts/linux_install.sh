#copy exe into /bin
cp build/fuzzy-database /bin
#copy init.d script to the right place
cp scripts/linux_daemon.sh /etc/init.d
mv /etc/init.d/scripts/linux_daemon.sh /etc/init.d/scripts/FZDB
#set permissons on init.d script
chmod uga+x /etc/init.d/scripts/FZDB
#database config file - /etc/fzdb/config.cnf
mdkir /etc/fzdb
cp scripts/linux_config.cnf /etc/fzdb
mv /etc/fzdb/linux_config.cnf /etc/fzdb/default.cnf
#set to start at system start
update-rc.d FZDB defaults
#touch log file
touch /var/log/FZDB.log
#mkdir data/fzdb
mkdir /data/fzdb