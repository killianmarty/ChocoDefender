#!/bin/bash

if [ "$EUID" -ne 0 ]; then 
    echo "Please run as root"
    exit 1
fi

read -p "Enter banned keyword (default: choco): " KEYWORD
KEYWORD=${KEYWORD:-choco}

echo "--- Starting Installation ---"

# 2. Compile project
echo "Compiling..."
make clean
make
if [ $? -ne 0 ]; then
    echo "Make failed. Aborting."
    exit 1
fi

echo "Installing binary to /usr/local/bin/chocodefender..."
cp bin/ChocoDefender /usr/local/bin/chocodefender
chmod +x /usr/local/bin/chocodefender

echo "Generating /etc/systemd/system/ChocoDefender.service..."
cat > /etc/systemd/system/ChocoDefender.service <<EOF
[Unit]
Description=ChocoDefender Background Service
After=multi-user.target

[Service]
Type=simple
ExecStart=/usr/local/bin/chocodefender "$KEYWORD"
Restart=always
RestartSec=3
User=root
Group=root
StandardOutput=null
StandardError=journal

[Install]
WantedBy=multi-user.target
EOF

echo "Reloading systemd daemon..."
systemctl daemon-reload
systemctl enable ChocoDefender
systemctl restart ChocoDefender

echo "--- Installation Complete ---"
systemctl status ChocoDefender --no-pager