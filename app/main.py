import sys
import os
import glob
import importlib
import types


class Engine():
    def __init__(self):
        self.current_strategy = ''
        self.init_run = True
        self.run_num = 0
    def start(self):
        self.run_num += 1
        self.run()
        self.init_run = False
        for r in range(10):
            self.run_num += 1
            self.run()
            
e = Engine()


def main():
    print('running main...')
    modules = {}
    DIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    sys.path.insert(0,f'{DIR}/strategies')

    for f in glob.glob(f'{DIR}/strategies/*.py'):
        name = os.path.basename(f)[:-3]
        modules[name] = importlib.import_module(name)
    
    e.current_strategy = 'hello'
    e.run = types.MethodType(modules['hello'].run, e)

    e.start()
    



if __name__ == '__main__':
    main()
