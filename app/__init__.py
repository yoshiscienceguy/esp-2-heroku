import flask,requests,json 
import jinja2
from authlib.flask.client import OAuth

app = flask.Flask(__name__)

number = 0
temperature = 0

@app.route("/")
def home_view():
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
    global number
    number += 1
    return flask.jsonify(
                status=200
            )

@app.route("/temp.html", methods=["POST"])
def temp():
    global temperature
    
    info = 0
    print(flask.request.args)
    print(flask.request.form)
    print(flask.request.form)
    tempString = json.loads(info)
    
    temperature = tempString.get("info")
    return flask.jsonify(
                status=200
            )
