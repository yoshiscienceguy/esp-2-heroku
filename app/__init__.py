import flask,requests,json 
import jinja2
from authlib.flask.client import OAuth

app = flask.Flask(__name__)

@app.route("/")
def home_view():
    return "<p>hello</p>"


@app.route("/abc.html", methods=["GET"])
def abc():
    return flask.jsonify(
                message="good test",
                category="success",
                data="yes",
                status=200
            )
