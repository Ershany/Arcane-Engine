import os
import subprocess
import platform

# Check our python requirements to make sure everything is installed
from PythonSetup import PythonConfiguration as PythonRequirements
PythonRequirements.Validate()

# Check our premake requirements to make sure everything is installed
from PremakeSetup import PremakeConfiguration as PremakeRequirements
os.chdir('./../') # Changes our directory from Setup to the root of the project

premakeInstalled = PremakeRequirements.Validate()

# print("\nUpdating submodules..")
# subprocess.call(["git", "submodule", "update", "update", "--init", "--recursive"])

if premakeInstalled:
	if platform.system() == "Windows":
		print("\nRunning premake..")
		subprocess.call([os.path.abspath("./Setup/Arcane-Windows-GenerateProjects.bat"), "nopause"])
		
	print("\nSetup completed")
else:
	print("Arcane requires Premake to generate the project files")