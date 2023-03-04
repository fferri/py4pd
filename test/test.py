def runTest():
    import os
    import subprocess
    import sys
    import platform
    
    if platform.system() == 'Linux':
        cmd = 'pd -nogui -send "start-test bang"  py4pd_Linux/test.pd'
        output = subprocess.run(cmd, capture_output=True, text=True, shell=True)
        outputLines = str(output).split('\\n')
        lastLine = outputLines[-2]
        
    elif platform.system() == 'Windows':
        scriptfile = os.path.abspath(__file__)
        scriptfolder = os.path.dirname(scriptfile)
        pathfile = scriptfolder + '\\test.pd'
        output = subprocess.run(f'"C:\\Program Files\\Pd\\bin\\pd.exe" -nogui -send "start-test bang" "{pathfile}"', capture_output=True, text=True, shell=True)
        outputLines = str(output).split('\\n')
        lastLine = outputLines[-2]
    elif platform.system() == 'Darwin':
        cmd = '/Applications/Pd-*.app/Contents/Resources/bin/pd -nogui -send "start-test bang" py4pd_macOS-Intel/test.pd'
        output = subprocess.run(cmd, capture_output=True, text=True, shell=True)
        outputLines = str(output).split('\\n')
        lastLine = outputLines[-2]
    # if lastLine contains "PASS" then the test passed
    if "PASS" in lastLine:
        # print in green
        print("\033[92m" + ' ALL TESTS PASSED ' + "\033[0m")
        return "ok"
    else:
        # split all the lines
        for line in outputLines:
            # if the line contains "FAIL" then print in red
            if "FAIL" in line:
                print("\033[91m" + line + "\033[0m")
            # if the line contains "PASS" then print in green
            elif "PASS" in line:
                print("\033[92m" + line + "\033[0m")
            # otherwise print normally
            else:
                print(line)
        sys.exit(1)
    
if __name__ == "__main__":
    runTest()
    
         
              

