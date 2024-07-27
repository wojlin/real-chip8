#!/bin/bash

# Define the script name and its directory
SCRIPT_NAME="runtime.py"
SCRIPT_DIR="$(dirname "$(realpath "$0")")"  # Get the directory of the current script
SERVICE_NAME="usb-monitor-emulator.service"

# Create a systemd service file
SERVICE_FILE="/etc/systemd/system/$SERVICE_NAME"

# Check if the service file already exists
if [ -f "$SERVICE_FILE" ]; then
    echo "Service file $SERVICE_FILE already exists. Exiting."
    exit 1
fi

# Create the systemd service file
echo "Creating systemd service file $SERVICE_FILE"
sudo tee "$SERVICE_FILE" > /dev/null <<EOF
[Unit]
Description=USB Monitor Emulator Service
After=network.target

[Service]
ExecStart=/usr/bin/python3 $SCRIPT_DIR/$SCRIPT_NAME
WorkingDirectory=$SCRIPT_DIR
Restart=always
User=$(whoami)

[Install]
WantedBy=multi-user.target
EOF

# Reload systemd to recognize the new service
echo "Reloading systemd daemon"
sudo systemctl daemon-reload

# Enable the service to start on boot
echo "Enabling the service to start on boot"
sudo systemctl enable "$SERVICE_NAME"

# Start the service immediately
echo "Starting the service now"
sudo systemctl start "$SERVICE_NAME"

echo "Setup complete. The Python script will now run at startup."
