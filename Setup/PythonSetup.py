import sys
import subprocess
import importlib.util as importlib_util

class PythonConfiguration:
	@classmethod
	def Validate(config):
		if not config.__ValidatePython():
			return # cannot validate
		
		for packageName in ["requests"]:
			if not config.__ValidatePackage(packageName):
				return # cannot validate

	@classmethod
	def __ValidatePython(config, versionMajorRequired=3, versionMinorRequired=3):
		if sys.version is not None:
			print("Python Version {0:d}.{1:d}.{2:d} detected".format( \
				sys.version_info.major, sys.version_info.minor, sys.version_info.micro))
			if sys.version_info.major < versionMajorRequired or (sys.version_info.major == versionMajorRequired and sys.version_info.minor < versionMinorRequired):
				print("Python Version is too low, expected version {0:d}.{1:d} or higher".format( \
					versionMajorRequired, versionMinorRequired))
				return False
			return True
		return False

	@classmethod
	def __ValidatePackage(config, packageName):
		if importlib_util.find_spec(packageName) is None:
			return config.__InstallPackage(packageName)
		return True

	@classmethod
	def __InstallPackage(config, packageName):
		permissionGranted = False
		while not permissionGranted:
			reply = str(input("Would you like to install Python package '{0:s}'? [Y/N]: ".format(packageName))).lower().strip()[:1]
			if reply == 'n':
				return False
			permissionGranted = (reply == 'y')
		
		print(f"Installing {packageName} module..")
		subprocess.check_call(['python', '-m', 'pip', 'install', packageName])
		
		return config.__ValidatePackage(packageName)

if __name__ == "__main__":
	PythonConfiguration.Validate()