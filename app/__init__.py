import flask,requests,json, os
import jinja2
from authlib.flask.client import OAuth
from flask_sqlalchemy import SQLAlchemy


app = flask.Flask(__name__)

app.config['SQLALCHEMY_DATABASE_URI'] = 'sqlite:///test.db'
app.config['SQLALCHEMY_TRACK_MODIFICATIONS'] = 'False'
db = SQLAlchemy(app)

#db.create_all() to create
class Info(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    temp = db.Column(db.String(5), unique=False, nullable=False)
    status = db.Column(db.String(7), unique=False, nullable=False)

    def __repr__(self):
        return '<Temp %r , Status %r>' % (self.temp, self.status)

@app.route("/")
def home_view():
    toReturn = ""
    allInfo = Info.query.all()
    for infoBit in allInfo:
        toReturn += "<p> Status: %s , Temperature: %s</p><br>"%(infoBit.status,infoBit.temp)
    
    return toReturn

@app.route("/latest.html", methods=["GET"])
def latest():
    latest = Info.query.order_by(Info.id.desc()).first()
    if(latest == None):
        return flask.jsonify(info="No Data",status=200)
    else:  
        return flask.jsonify(info=latest.status + ";" + latest.temp,status=200) 

@app.route("/toggle")
def toggle_Manual():
    oldInfo = Info.query.order_by(Info.id.desc()).first()
    if(oldInfo == None):
        temperature = "0"
        stat = "0"
    else:
        temperature = oldInfo.temp
        stat = oldInfo.status

    if(stat == "0"):
        stat = "1"
    else:
        stat = "0"
        
    newInfo = Info(temp = temperature , status = stat)
    db.session.add(newInfo)
    db.session.commit()
    return flask.jsonify(status=200)

@app.route("/toggle.html", methods=["POST"])
def toggle():
    oldInfo = Info.query.order_by(Info.id.desc()).first()
    if(oldInfo == None):
        temperature = "0"
        stat = "0"
    else:
        temperature = oldInfo.temp
        stat = oldInfo.status

    if(stat == "0"):
        stat = "1"
    else:
        stat = "0"
        
    newInfo = Info(temp = temperature , status = stat)
    db.session.add(newInfo)
    db.session.commit()
    return flask.jsonify(status=200)

@app.route("/temp.html", methods=["POST"])
def temp():

    newTemp = list(flask.request.form.keys())[0]
    
 
    
    oldInfo = Info.query.order_by(Info.id.desc()).first()
    if(oldInfo == None):
        temperature = "0"
        stat = "0"
    else:
        temperature = oldInfo.temp
        stat = oldInfo.status

        
    newInfo = Info(temp = newTemp , status = stat)
    db.session.add(newInfo)
    db.session.commit()
    return flask.jsonify(
                status=200
            )
