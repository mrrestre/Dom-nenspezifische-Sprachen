import subprocess
import os
import json

class Compiler:
    def __init__(self, tokenstream_path, compiled_path):
        self.tokenstream_path = tokenstream_path
        self.compiled_path = compiled_path
        self.compiled_code = {}
        self.parser = 'dsm_lemon/parse.exe'

    def compile(self):
        try:
            with open(self.tokenstream_path, 'r', encoding='utf-8') as file:
                input_data = file.read()
            
            result = subprocess.run([self.parser], input=input_data, stdout=subprocess.PIPE, stderr=subprocess.PIPE, text=True, check=True)
            self.compiled_code = json.loads(result.stdout)
            
            # Save to a JSON file
            with open(self.compiled_path, 'w', encoding='utf-8') as file:
                json.dump(self.compiled_code, file, indent=4)
        
        except subprocess.CalledProcessError as e:
            print(f"Error occurred: {e.stderr}")
            raise
        except FileNotFoundError:
            print(f"File not found: {self.tokenstream_path}")
            raise
        except json.JSONDecodeError as e:
            print(f"Error decoding JSON: {e}")
            raise