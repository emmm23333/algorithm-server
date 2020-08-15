## MQTT协议
算法服务连接mqtt
```
UserName: testuser 
Password: testpassword
CleanStart: false
ClientId: uuid

说明: 待补充
```

## mqtt消息格式约定

### 任务消息格式
```json
{
	"id": "uuid",
	"method": "xxx", // 详见配置中的flow部分的key名称
	"backMsg":"xxx",
	"targetFormat":"",
	"flow": {
		"1": ["repair","simplify","smooth","saveGlb"],
		"2": ["saveStl"],
		"3": ["pngFile"]
	},
	"params": {
		"oss":{
			"bucket": {
				"data":"t-3datahz",
				"pub":"xxx"
			},
			"dfsId": "dental/3mf_test_1"
		},
		"config": {
			"readOp": {
				"format": "3mf",
				"isZip":false
			},
			"3mfOp":{
				"withColor": false,
				"withTexture": false
			},
			"objOp": {
				"readVColorIfExists": false
			},
			"simplify": {
				"remainRate": 0.9,
				"boundaryWeight": 3.45,
				"isCheckFaceFlip": false,
				"isFeatureSensitive": false
			},
			"smooth": {
				"moveDist": 3.45,
				"iter": 5
			},
			"saveGlb": {
				"needUpload": true,
				"writeASCII": false,
				"pubBucket": true
			},
			"saveStl": {
				"needUpload": true,
				"pubBucket": false
			},
			"savePly": {
				"needUpload": true,
				"pubBucket": false,
				"writeASCII": false
			},
			"save3mf": {
				"needUpload": true,
				"pubBucket": false
			},
			"saveBeb": {
				"needUpload": true,
				"pubBucket": false
			},
			"saveObj": {
				"needUpload": true,
				"writeVColorIfExists": false,
				"pubBucket": false
			},
			"repair": {
				"maxRetries":5
			},
			"saveZip": {
				"needUpload": true,
				"replaceZip":true,
				"pubBucket": false
			},
			"pngFile": [{
				"width": 10,
				"height": 10,
				"longitude": 23.455,
				"latitude": 55.66,
				"needUpload": true,
				"pubBucket": true
			}]
		}
	}
}
```

- 说明：
	- pngFile字段可选，不存在取默认参数
		- 如果有多个文件，然后pngFile的数组又有多个对象，则表示每个文件，都循环数组进行渲染保存上传
    - saveGlb字段可选，不存在取默认参数
    - saveObj字段可选，不存在取默认参数，存在时writeVColorIfExists可选，不存在则从配置文件取，配置文件中不存在则默认false
    - repair字段可选，读取规则同上
	- backMsg是状态及结果消息必须要返回的内容

## 任务状态及结果消息格式
```json
{
	"id": "uuid",
	"method": "xxx", // 详见配置中的flow部分的key名称
	"backMsg":"xxx",
	"targetFormat":"xxx",
	"status":"xxx",
	"error": "",
	"srcParams":"xxx", // TODO...消息原始中的params中的内容压成字符串
	"result": {
		"pngFile": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}], // 大小为字节
		"saveStl": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}],
		"saveObj": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}],
		"saveGlb": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}],
		"savePly": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}],
		"saveBeb": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}],
		"saveZip": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}],
		"save3mf": [{"dfsId":"xxx","endpoint":"xxx","bucket":"xxx","size":1024,"isPub":true}],
		"computeResult":{
			"badEdgeCount":0,
			"tinyHolesCount":0,
			"bigHolesCount":0,
			"badOriTriCount":0,
			"isctTriCount":550,
			"faceCount":4548,
			"vertCount":2296,
			"edgeCount":6822,
			"xLen":52.06775665283203,
			"yLen":18.757768630981445,
			"zLen":50.42413330078125,
			"retries":0
		},
		"finalComputeResult":{
			"badEdgeCount":0,
			"tinyHolesCount":0,
			"bigHolesCount":0,
			"badOriTriCount":0,
			"isctTriCount":550,
			"faceCount":4548,
			"vertCount":2296,
			"edgeCount":6822,
			"xLen":52.06775665283203,
			"yLen":18.757768630981445,
			"zLen":50.42413330078125,
			"retries":0
		}
	} 
}
```
- 说明：
	- 如果status不是success，则result可以不传
	- 如果status不是error，则error可以为空或不传
	- result中的各项，根据不同流程设置返回
- status:
	- overflow 表示该机任务已经满载，需要重发
	- error 表示出错了
	- success 处理成功

## 配置文件(.config)
```json
{
	"globle": {
		"log": {
			"path": "/tmp/log",
			"prefix": "algorithmservice",
			"level": 0,
			"output": 0
		},
		"savePath": {
			"msg": "tmp/task",
			"download": "download",
			"newSave": "new_save"
		},
		"mqtt": {
			"host": "127.0.0.1",
			"port": 8882,
			"username": "testuser",
			"password": "testpasswrod",
			"cleanStart": false,
			"clientId": "12345",
			"receive": "service/task/new",
			"send": "service/task/result"
		},
		"control": {
			"processes": 5,
			"cpu": 80,
			"netRetryInterval": 1,
			"netRetries": 2,
			"uploadPartSize": 8388606,
			"uploadThreadNum": 1
		},
		"oss":{
			"t-3data":{
				"endpoint": "",
				"accessKey": "",
				"accessSecret": ""
			},
			"t-3datahz":{
				"endpoint": "",
				"accessKey": "",
				"accessSecret": ""
			},
			"t-pub":{
				"endpoint": "",
				"accessKey": "",
				"accessSecret": ""
			}
		}
	},
	"modules": {
		"readOp": {
			"format": "3mf",
			"isZip": true
		},
		"3mfOp": {
			"withColor": false,
			"withTexture": false
		},
		"objOp": {
			"readVColorIfExists": false
		},
		"simplify": {
			"remainRate": 0.9,
			"boundaryWeight": 3.45,
			"isCheckFaceFlip": false,
			"isFeatureSensitive": false
		},
		"smooth": {
			"moveDist": 3.45,
			"iter": 5
		},
		"saveGlb": {
			"needUpload": true,
			"writeASCII": false,
			"pubBucket": true
		},
		"saveStl": {
			"needUpload": true,
			"pubBucket": false
		},
		"savePly": {
			"needUpload": true,
			"pubBucket": false,
			"writeASCII": false
		},
		"saveBeb": {
			"needUpload": true,
			"pubBucket": false
		},
		"save3mf": {
			"needUpload": true,
			"pubBucket": false
		},
		"saveObj": {
			"writeVColorIfExists": false,
			"needUpload": true,
			"pubBucket": false
		},
		"saveZip": {
			"replaceZip": false,
			"needUpload": true,
			"pubBucket": false
		},
		"repair": {
			"maxRetries": 5
		},
		"pngFile": {
			"width": 10,
			"height": 10,
			"longitude": 23.455,
			"latitude": 55.66,
			"needUpload": true,
			"pubBucket": true
		}
	},
	"flow": {
		"preview_convert_stl": {
			"1": ["repair","simplify","smooth","saveGlb"],
			"2": ["saveStl"],
			"3": ["pngFile"]
		},
		"preview_convert_obj": {
			"1": ["repair","simplify","smooth","saveGlb"],
			"2": ["saveObj"],
			"3": ["pngFile"]
		},
		"preview_convert_beb": {
			"1": ["repair","simplify","smooth","saveGlb"],
			"2": ["saveBeb"],
			"3": ["pngFile"]
		},
		"preview_convert_ply": {
			"1": ["repair","simplify","smooth","saveGlb"],
			"2": ["savePly"],
			"3": ["pngFile"]
		},
		"dental_preview_convert_stl": {
			"1": ["repair","simplify","smooth","saveGlb"],
			"2": ["saveStl"],
			"3": ["pngFile"]
		},
		"dental_preview_convert_obj": {
			"1": ["repair","simplify","smooth","saveGlb"],
			"2": ["saveObj"],
			"3": ["pngFile"]
		},
		"preview": {
			"1": ["simplify","smooth","saveGlb"],
			"2": ["pngFile"]
		},
		"convert_stl": {
			"1": ["saveStl"],
			"2": ["pngFile"]
		},
		"convert_obj": {
			"1": ["saveObj"],
			"2": ["pngFile"]
		},
		"convert_beb": {
			"1": ["saveBeb"],
			"2": ["pngFile"]
		},
		"convert_ply": {
			"1": ["savePly"],
			"2": ["pngFile"]
		},
		"convert_3mf": {
			"1": ["save3mf"],
			"2": ["pngFile"]
		},
		"repair_stl": {
			"1": ["compute","repair","saveStl"],
			"2": ["pngFile"]
		},
		"repair_obj": {
			"1": ["compute","repair","saveObj"],
			"2": ["pngFile"]
		},
		"repair_beb": {
			"1": ["compute","repair","saveBeb"],
			"2": ["pngFile"]
		},
		"repair_ply": {
			"1": ["compute","repair","savePly"],
			"2": ["pngFile"]
		},
		"repair_3mf": {
			"1": ["compute","repair","save3mf"],
			"2": ["pngFile"]
		},
		"screenshot": {
			"1": ["pngFile"]
		},
		"compute": {
			"1": ["compute"],
			"2": ["pngFile"]
		}
	}
}

```
- 说明：
	1. log level:0表示不过滤；16为Trace级别；64为Debug级别
    2. log output:0表示不过滤；1为输出至文件；2为输出至终端
	3. repair接口调用之前先判断计算结果（不要保存到compute result），如果结果中BadEdgeCount\BadOriTriCount的数量还大于0并且未超过最大尝试次数，则要进行修复。修复完成时的计算结果(作为finalComputeResult)，这2个结果都要随消息返回
	4. 一般数据都是从oss的data属性的bucket中下载，但处理好的结果则要根据参数与配置进行，在needUpload为true的时候，如果pubBucket为true，则提取消息中的pub的bucket来保存处理结果，否则保存至data的bucket中
	5. compute调用的是get_mesh_error_info，其结果要在消息中返回
	6. 流程设置明确每个流程要穷举出其各种执行路径，每种路径为1个子流程，子流程中按顺序执行。目前的流程要注意不能出现循环的情况；
	7. 注意compute的结果作为computeResult

# other

- ~~加入一个字段：执行前的依赖，解决执行顺序问题，有些可以并发。~~
- ~~格式参照钉钉发的~~
- 响应的格式：
- 生成的dfsid路径前缀使用原路径+原始文件名+ "_out/"+ 保存类型 +"/"+ 新生成的唯一名
```
dental/aaa
pngFile:
dental/aaa_out/pngFile/20190708123456432
```
- 响应的队列名称在配置文件里配置，预览文件地址在zip下是多个的


- ~~加入子进程数量的设置~~

# 如果任务的isZip/replaceZip是true，并且saveZip的needUpload为true的时候，在进程的内存中建立类似下面的结构，该进程中所有线程执行完成时根据这个结构进行循环删除原zip中与key对应的文件，并且把该文件在进程中产生的所有文件放到同目录下，最后再压缩成zip上传
# zip压缩包下可能有多级目录。根据readop里面的format，只处理那种类型的。保持目录结构不变。

```json
[{
	"aaa.beb":["aa.stl","sss.png"],
	"bbb.beb":["aa","sss"],
	"ccc.beb":["aa","sss"],
	"ddd.beb":["aa","sss"],
	"eee.beb":["aa","sss"]
}]
aaa.beb可以原带文件名的路径
```

obj如果生成多个文件的话，打包成zip，这个的dfsid还是用saveObj字段。
齿科的话那就是zip中有zip了

~~1. 算法补全（/ply读写、上传/) （这一版）（0.5-1~~

~~2. 进程状态管理（这一版 overflow）（简单版本0.5-1~~

~~3. 异常处理（3-5~~
（1）算法报错要返回错误信息（报错要中止任务） （这一版）（0.5-1）
（2）进程崩溃要补充进程，记录 （这一版）
（3）主进程监测到进程崩溃，发消息 （这一版）

~~遇到过没解决的问题 ：1.算法崩过一次 2.主进程按回车键收到重复消息 3.进程崩溃时信号没捕获到 4.mqtt连接时没订阅上（这一版）~~

0. 重构（代码结构、进程选型 下一版）一周考虑重构方案

1. 任务记录
	开始前保存，执行完成后删除，启动时继续执行未被删除的任务。（下一版）

2. make改成cmake（下一版）

4. 文件清理（下载的和生成的文件）（下一版）

5. cpu使用率控制（下一版）

6. 资源监控（下一版）

7. bdd（下一版 包含业务流程）

8. 分块断点续传，并发上传（下一版）

9. 任务超过一定时间（比如上传时网络不行）应该取消（下一版）

需求文档 （只参与）
设计文档（参与或者负责，评审）
估算时间


