from tokenizer import Tokenizer
from compiler import Compiler

folder = 'steps/'
input_file_path = folder + '01_input.txt'
token_file_path = folder + '02_tokenstream.txt'
compiled_file_path = folder + '03_compiled.txt'

tokenizer = Tokenizer(input_file_path, token_file_path)

compiler = Compiler(token_file_path, compiled_file_path)
try:
    output = compiler.compile()
except ValueError as ve:
    print(ve)