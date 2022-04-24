import sqlite3

def init_db():
  try:
    sqlite_connection = sqlite3.connect('weather.db')
    sqlite_create_table_query = '''CREATE TABLE weather_history (
                                dt datetime NOT NULL,
                                city text NOT NULL,
                                discription text NOT NULL,
                                value text NOT NULL);'''

    cursor = sqlite_connection.cursor()
    print("База данных подключена к SQLite")
    cursor.execute(sqlite_create_table_query)
    sqlite_connection.commit()
    print("Таблица SQLite создана")

    cursor.close()

  except sqlite3.Error as error:
    print("Ошибка при подключении к sqlite", error)
  finally:
    if (sqlite_connection):
        sqlite_connection.close()
        print("Соединение с SQLite закрыто")


def insert_val(vals=[]):
  if len(vals) == 4 :
    sqlite_connection = sqlite3.connect('weather.db',
                                        detect_types=sqlite3.PARSE_DECLTYPES |
                                        sqlite3.PARSE_COLNAMES)
    cursor = sqlite_connection.cursor()
    cursor.execute("INSERT INTO weather_history (dt, city, discription, value) VALUES (?,?,?,?)", vals)
    sqlite_connection.commit()


def check_db(dt, city):
  sqlite_connection = sqlite3.connect('weather.db',
                                      detect_types=sqlite3.PARSE_DECLTYPES |
                                                   sqlite3.PARSE_COLNAMES)
  cursor = sqlite_connection.cursor()
  sql="SELECT * FROM weather_history WHERE dt=? and city=?"
  cursor.execute(sql, [dt, city])
  res = cursor.fetchall()
  return res