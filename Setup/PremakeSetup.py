import sys
import os
from pathlib import Path

import ArcaneUtils

class PremakeConfiguration:
	premakeVersion = "5.0.0-beta1"
	premakeZipDownloadUrl = f"https://github.com/premake/premake-core/releases/download/v{premakeVersion}/premake-{premakeVersion}-windows.zip"
	premakeLicenseUrl = "https://raw.githubusercontent.com/premake/premake-core/master/LICENSE.txt"
	premakeDirectory = "./Dependencies/premake/bin"

	@classmethod
	def Validate(config):
		if not config.CheckIfPremakeIsInstalled():
			print("Premake is not installed")
			return False
			
		print(f"Correct Premake located at {os.path.abspath(config.premakeDirectory)}")
		return True

	@classmethod
	def CheckIfPremakeIsInstalled(config):
		premakeExe = Path(f"{config.premakeDirectory}/premake5.exe")
		if not premakeExe.exists():
			return config.InstallPremake()
			
		return True

	@classmethod
	def InstallPremake(config):
		permissionGranted = False
		while not permissionGranted:
			reply = str(input("Would you like to download Premake {0:s}? [Y/N]: ".format(config.premakeVersion))).lower().strip()[:1]
			if reply == 'n':
				return False
			permissionGranted = (reply == 'y')
			
		premakePath = f"{config.premakeDirectory}/premake-{config.premakeVersion}-windows.zip"
		print("Downloading {0:s} to {1:s}".format(config.premakeZipDownloadUrl, premakePath))
		ArcaneUtils.DownloadFile(config.premakeZipDownloadUrl, premakePath)
		print("Extracting", premakePath)
		ArcaneUtils.UnzipFile(premakePath, deleteZipFile=True)
		print(f"Premake {config.premakeVersion} has been downloaded to '{config.premakeDirectory}'")
		
		premakeLicensePath = f"{config.premakeDirectory}/LICENSE.txt"
		print("Downloading {0:s} to {1:s}".format(config.premakeLicenseUrl, premakeLicensePath))
		ArcaneUtils.DownloadFile(config.premakeLicenseUrl, premakeLicensePath)
		print(f"Premake License file has been downloaded to '{config.premakeDirectory}'")
		
		return True


if __name__ == "__main__":
	PremakeConfiguration.Validate()