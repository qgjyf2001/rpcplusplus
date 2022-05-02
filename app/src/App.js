import { useState } from 'react';
import { Line } from '@ant-design/plots';
import { Space,message,Card} from 'antd';
import ProForm,{ProFormText,ProFormDateTimeRangePicker} from '@ant-design/pro-form'
import { format } from 'react-string-format';
import './index.css'
const DemoLine = () => {
  const [data, setData] = useState([]);
  const onFinish = async (values) => {
    var url=format("/api/getDot?key={0}&from={1}&to={2}",values['dotName'],values['timeRange'][0],values['timeRange'][1])
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
                onFinish={onFinish}
              >
                <ProFormText
                  width="md"
                  name="dotName"
                  label="打点名称"
                  placeholder="请输入打点名称"
                />
                <ProFormDateTimeRangePicker
                  width="md"
                  name='timeRange'
                  label="起止时间"
                />
              </ProForm>
            </Card>
            <Line {...config} width={1000}/>
          </Space>;
};

export default DemoLine;
