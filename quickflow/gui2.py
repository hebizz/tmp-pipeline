import redis
import json

if __name__ == '__main__':
    hnumber = 0
    PIXEL_X = 32
    PIXEL_Y = 24
    redis = redis.Redis(host='192.168.0.123', port='6379', decode_responses=True, password='jiangxing')
    while True:
        table_name = 'hashinfo_' + str(hnumber)
        info = redis.hgetall(table_name)
        if info:
            if 'ID' in info and 'XY' in info:
                car_id = info['ID']
                point = json.loads(info['XY'])
                # angle = info['Angle']
                # self.InputBox2.set_value("car location info :" + str(info))
                mark = ''
                x = point[0] // 20
                y = point[1] // 20
                if x >= PIXEL_X:
                    x = PIXEL_X
                if y >= PIXEL_Y:
                    y = PIXEL_Y

                for k in range(PIXEL_Y):
                    for i in range(PIXEL_X):
                        if k == y and i == x:
                            mark += '@'
                        else:
                            mark += "-"
                    mark = mark + '\n'

                print(mark, end='')
                # print(mark + '\n')
                # self.InputBox1.set_value(mark + '\n')
                hnumber += 1
