import os, sys

userhome = os.path.expanduser('~')

def load():
    try:
        dirs = os.environ['DSSI_PATH'].split(os.pathsep)
    except KeyError:
        dirs = [os.path.join(userhome, '.dssi')] # Skip system paths for now.
    for dir in dirs:
        print('Scanning:', dir, file = sys.stderr)
        path = os.path.join(dir, 'libcarstairs.py')
        if os.path.exists(path):
            print('Reading config:', path, file = sys.stderr)
            with open(path) as f:
                return f.read()

exec(load())
