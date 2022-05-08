import { useState } from 'react';
import { Line } from '@ant-design/plots';
import { Space, message, Card, Button } from 'antd';
import ProForm, { ProFormText, ProFormDateTimeRangePicker } from '@ant-design/pro-form'
import { format } from 'react-string-format';
import moment from 'moment'
import './index.css'
const DemoLine = () => {
  const [data, setData] = useState([]);
  const [toNow, setToNow] = useState(null);
  const onFinish = async (values) => {
    var url = format("/api/getDot?key={0}&from={1}&to={2}", values['dotName'], values['startTime'], values['endTime'])
    console.log(url)
    fetch(url)
      .then((response) => response.json())
      .then((json) => {
        console.log(json)
        setData(json)
      })
      .catch((error) => {
        message.error("网络错误")
      });
  };
  const config = {
    data,
    padding: 'auto',
    xField: 'time',
    yField: 'count',
    xAxis: {
      // type: 'timeCat',
      tickCount: 5,
    },
  };

  return <Space direction="horizontal">
    <Card style={{ width: 550 }}>
      <ProForm
        syncToUrl={(values, type) => {
          if (type === 'get') {
            return {
              ...values,
              timeRange:
                values.startTime || values.endTime ? [values.startTime, values.endTime] : undefined,
            };
          }
          return {
            ...values
          };
        }}
        onFinish={onFinish}
        autoFocusFirstInput
      >
        <ProFormText
          width="md"
          name="dotName"
          label="打点名称"
          placeholder="请输入打点名称"
        />
        <ProFormDateTimeRangePicker
          convertValue={(values) => {
            if (values === undefined) {
              return values
            }
            return [values[0], values[1] === "now" ? moment().format('YYYY-MM-DD HH:mm:ss') : values[1]]
          }}
          transform={(values) => {
            return {
              startTime: values ? values[0] : undefined,
              endTime: toNow ? toNow : (values ? values[1] : undefined),
            };
          }}
          width="md"
          name='timeRange'
          extra={<Button onClick={() => { setToNow("now") }}>to now</Button>}
          label="起止时间"
        />
      </ProForm>
    </Card>
    <Line {...config} width={1000} />
  </Space>;
};

export default DemoLine;
