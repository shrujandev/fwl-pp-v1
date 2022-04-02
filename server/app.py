import json
import os
from datetime import datetime

import firebase_admin
from firebase_admin import credentials, firestore
from flask import Flask, Response, request

app = Flask(__name__)

PROJECT_ID = os.environ.get('PROJECT_ID')
# Use the application default credentials
cred = credentials.Certificate('./serviceAccountKey.json')
firebase_admin.initialize_app(cred, {
  'projectId': PROJECT_ID,
})

db = firestore.client()


@app.route('/upload', methods=['POST'])
def upload():
    if (request.method == 'POST'):
        data = request.get_json()

        # validate data
        if (not data.get('rfid') or not data.get('laser') or not data.get('force')):
            return Response(status=400)
        else :
            # upload data to db
            data['timestamp'] = str(datetime.now())
            db.collection(u'sensordata').document().set(data)         
            return Response(json.dumps(data), mimetype='application/json', status=200)
    return 'NOT POST'


