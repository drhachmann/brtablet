boot_SCRIPTS = brtablet-init.sh
bootdir = /etc/init.d

install-data-local:
	sudo update-rc.d brtablet-init.sh defaults 3
install-post:
	sudo update-rc.d brtablet-init.sh defaults 3

uninstall-local:
	sudo update-rc.d -f brtablet-init.sh remove
