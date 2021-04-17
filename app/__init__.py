import flask,requests,json 
import jinja2
from authlib.flask.client import OAuth

app = flask.Flask(__name__)

number = 0

@app.route("/")
def home_view():
    return str(number)


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
    number += 1
    return flask.jsonify(
                status=200
            )
