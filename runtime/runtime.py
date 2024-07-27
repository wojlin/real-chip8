import psutil
import time
import os
import platform
import re
import subprocess
import copy


class Runtime:
    DRIVE_NAME = "chip8"
    PROGRAM_ARGS = ["../emulator/build/chip8", "--ui", "window", "--type", "raw", "--data"]
    CHECK_DELAY = 1

    def __init__(self):
        self.drives = self.get_current_drives()
        self.added_drives = {}
        self.removed_drives = {}
        self.drive_labels = {}  # Dictionary to store drive labels
        self.ch8_files = {}  # Dictionary to store .ch8 files
        self.emulator_process = None  # Variable to store the emulator process

        # Detect if the desired USB drive is already present on initialization
        self.initial_drive_check()

        self.run()

    def initial_drive_check(self):
        for drive, mountpoint in self.drives.items():
            drive_label = self.get_drive_label(drive)
            self.drive_labels[drive] = drive_label
            if drive_label == Runtime.DRIVE_NAME:
                print("Detected desired USB drive at startup.")
                self.load_ch8_files(mountpoint)
                self.launch_emulator()

    def manage_plugged(self, new_drives):
        for drive in self.added_drives:
            drive_label = self.get_drive_label(drive)
            self.drive_labels[drive] = drive_label  # Store the drive label
            print(f"New USB drive detected: {drive} (Label: {drive_label})")

            if drive_label == Runtime.DRIVE_NAME:
                print("Detected desired USB drive")
                self.load_ch8_files(new_drives[drive])
                self.launch_emulator()
                break

    def manage_unplugged(self, new_drives):
        for drive in self.removed_drives:
            drive_label = self.drive_labels.get(drive, "Unknown")  # Get the stored drive label
            print(f"USB drive removed: {drive} (Label: {drive_label})")
            if drive_label == Runtime.DRIVE_NAME:
                print("Unplugged desired USB drive")
                self.terminate_emulator()
                break

    def run(self):
        print("Monitoring for USB drive changes. Press Ctrl+C to stop.")
        try:
            while True:
                time.sleep(Runtime.CHECK_DELAY)
                new_drives = self.get_current_drives()
                self.added_drives, self.removed_drives = self.detect_changes(self.drives, new_drives)

                self.manage_plugged(new_drives)
                self.manage_unplugged(new_drives)

                self.drives = new_drives
        except KeyboardInterrupt:
            print("Monitoring stopped.")
            self.terminate_emulator()

    @staticmethod
    def get_current_drives():
        return {disk.device: disk.mountpoint for disk in psutil.disk_partitions()}

    @staticmethod
    def detect_changes(old_drives, new_drives):
        added = new_drives.keys() - old_drives.keys()
        removed = old_drives.keys() - new_drives.keys()
        return added, removed

    def get_drive_label(self, drive_device):
        try:
            if platform.system() == 'Windows':
                output = os.popen(f'vol {drive_device}').read()
                match = re.search(r'Volume in drive [A-Z]: is (.+)', output)
                return match.group(1) if match else "Unknown"
            elif platform.system() == 'Darwin':  # macOS
                output = os.popen(f'diskutil info {drive_device}').read()
                match = re.search(r'Volume Name:\s*(.+)', output)
                return match.group(1).strip() if match else "Unknown"
            elif platform.system() == 'Linux':
                output = os.popen(f'lsblk -no LABEL {drive_device}').read().strip()
                return output if output else "Unknown"
        except Exception as e:
            return "Unknown"

    def load_ch8_files(self, mountpoint):
        self.ch8_files = {}  # Reset the dictionary
        for root, dirs, files in os.walk(mountpoint):
            for file in files:
                if file.endswith('.ch8'):
                    file_path = os.path.join(root, file)
                    with open(file_path, 'rb') as f:
                        binary_content = f.read()
                        formatted_content = ' '.join(f'{byte:02x}' for byte in binary_content)
                        self.ch8_files[file] = formatted_content
        print("Loaded .ch8 files:", list(self.ch8_files.keys()))

    def launch_emulator(self):
        if self.emulator_process is None:
            content = self.choose_chip8_program()
            if content:
                data = copy.copy(Runtime.PROGRAM_ARGS)
                data.append(content)
                print(f"Launching emulator with arguments: {data}")
                # Uncomment the line below when ready to launch the real emulator
                # self.emulator_process = subprocess.Popen(data)
                self.emulator_process = subprocess.Popen(['sleep', '300'])  # Dummy process for demonstration
                print(f"Launched emulator program with PID: {self.emulator_process.pid}")

    def terminate_emulator(self):
        if self.emulator_process:
            self.emulator_process.terminate()
            try:
                self.emulator_process.wait(timeout=5)  # Wait for the process to terminate
                print(f"Terminated emulator program with PID: {self.emulator_process.pid}")
            except subprocess.TimeoutExpired:
                self.emulator_process.kill()
                print(f"Killed emulator program with PID: {self.emulator_process.pid}")
            self.emulator_process = None

    def choose_chip8_program(self):
        if self.ch8_files:
            first_file = next(iter(self.ch8_files))  # Get the first key from the dictionary
            first_file_content = self.ch8_files[first_file]
            print(f"First CHIP-8 program chosen: {first_file}")
            return first_file_content
        else:
            print("No .ch8 files found.")
            return None


def main():
    runtime = Runtime()


if __name__ == "__main__":
    main()
