import flask,requests,json, os
import jinja2
from authlib.flask.client import OAuth

app = flask.Flask(__name__)




os.environ['number'] = str(0)
os.environ['temperature'] = str(0)

@app.route("/")
def home_view():
    number = os.getenv('number')
    temperature = os.getenv('temperature')
    return "number: " + str(number) +  " , temperature" + str(temperature)


@app.route("/abc.html", methods=["GET"])
def abc():
    return flask.jsonify(
                message="good test",
                category="success",
                data="yes",
                status=200
            )

@app.route("/add.html", methods=["POST"])
def add():
    number = int(os.getenv('number'))
    number += 1
    os.environ['number'] = str(number)
    return flask.jsonify(
                status=200
            )

@app.route("/temp.html", methods=["POST"])
def temp():

    info = list(flask.request.form.keys())[0]
    
    os.environ['temperature']  = info
    
    return flask.jsonify(
                status=200
            )
