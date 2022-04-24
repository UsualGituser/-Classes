from flask import Flask, render_template, request, redirect, jsonify, abort
import db_init
import os.path
import datetime
import requests
from bs4 import BeautifulSoup

app = Flask(__name__)

def Get_gis(city = ''):
    if city == '':
      url = 'https://www.foreca.ru/'
    else:
        url = 'https://www.foreca.ru/Russia/'+city
    response = requests.get(url)
    if response.status_code == 200:
      return response.text
    else:
       return ""


def Parse_foreca(city=''):
    soup = BeautifulSoup(Get_gis(city), 'lxml')

    title = soup.find("h1", class_='entry-title')

    data = list(soup.find(class_='right txt-tight').children)
    n_list = [title.text]
    for item in data:
        item = str(item).strip()
        item = str(item).replace("<br/>", "")
        item = str(item).replace("<strong>", "")
        item = str(item).replace("</strong>", "")
        if str(item) != "":
            n_list.append(item)
    discript_list = [n_list[i] for i in range(0, len(n_list), 2)]
    value_list = [n_list[i + 1] for i in range(0, len(n_list), 2)]

    ans_list = [discript_list, value_list]

    return ans_list

def get_data(city):
    if not os.path.isfile('weather.db'):
        db_init.init_db()

    dt = datetime.datetime.now().strftime('%Y-%m-%d')

    db_vals = db_init.check_db(dt, city)
    if len(db_vals) == 0:
      vals = Parse_foreca(city)
      for i in range(len(vals[0])):
         ins_ls=[dt, city, vals[0][i], vals[1][i]]
         db_init.insert_val(ins_ls)
    else:
      vals=[]
      vals.append([db_vals[i][2] for i in range(len(db_vals))])
      vals.append([db_vals[i][3] for i in range(len(db_vals))])
    return vals

@app.route('/api/weather/<city>', methods=['GET'])
def get_weather(city):
    if len(city) == 0:
      abort(404)
    else:
      vals = get_data(city)
      return jsonify(vals)

@app.route('/result/<city>', methods=["POST", "GET"])
def weather(city):
    vals = get_data(city)
    return render_template("result.html",
                           citynm=vals[0][0],
                           d1=vals[0][0],
                           v1=vals[1][0],
                           d2=vals[0][1],
                           v2=vals[1][1],
                           d3=vals[0][2],
                           v3=vals[1][2],
                           d4=vals[0][3],
                           v4=vals[1][3],
                           d5=vals[0][4],
                           v5=vals[1][4],
                           d6=vals[0][5],
                           v6=vals[1][5],
                           d7=vals[0][6],
                           v7=vals[1][6],
                           d8=vals[0][7],
                           v8=vals[1][7]
                          )


@app.route('/', methods=["POST", "GET"])
def city():
   if request.method == "POST":
     cityname = request.form.get('cityname')
     return redirect(f"/result/{cityname}")
   return render_template('city.html')

if __name__ == '__main__':
    app.run()

