import sys
import os
import glob
import importlib
import types


class Algo():

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

e = Algo()


def run(name):
    print('running main...')
    DIR = os.path.dirname(os.path.abspath(sys.argv[0]))
    sys.path.insert(0,f'{DIR}/strategies')
    try:
        strategy = importlib.import_module(name)
    except:
        print(f'cannot import module {name}')
        return
        
    e.current_strategy = 'hello'
    e.run = types.MethodType(strategy.run, e)

    e.start()
    



if __name__ == '__main__':

    run(sys.argv[1])
