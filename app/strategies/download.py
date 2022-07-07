
def run(e):
	print(f'run: {e.run_num}, running download, strategy: {e.current_strategy}...')
	if e.init_run:
		print('init run...')
	