from tokenizer import Tokenizer
from compiler import Compiler
from interpreter import Interpreter

folder = 'steps/'
input_file_path = folder + '01_input.txt'
token_file_path = folder + '02_tokenstream.txt'
compiled_file_path = folder + '03_compiled.txt'

tokenizer = Tokenizer(input_file_path, token_file_path)

compiler = Compiler(token_file_path, compiled_file_path).compile()

interpreter = Interpreter(compiled_file_path).interpret()