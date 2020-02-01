set -e

gunicorn -k flask_sockets.worker api:app -w 6666