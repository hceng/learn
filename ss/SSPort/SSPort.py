# coding:utf - 8

import os
import time
import json
import random
import linecache
from time import sleep
from urllib.request import urlopen
from urllib.request import Request
from configparser import ConfigParser

from smtplib import SMTP
from email.mime.text import MIMEText
from email.header import Header


class APPData:
    def __init__(self):
        self.Intervals = None
        self.SendMAX = None
        self.ServerIP = None
        self.ServerPort = None
        self.SSConfigPath = None
        self.ServerNewPort = None


class WebData:
    def __init__(self):
        self.InternalURL = None
        self.ForeignURL = None
        self.InternalICMP = None
        self.InternalTCP = None
        self.ForeignICMP = None
        self.ForeignTCP = None


class MailData:
    def __init__(self):
        self.SMTPHost = None
        self.FromMail = None
        self.Password = None
        self.ToMail = None
        self.Subject = None
        self.Content = None


def getSSConfig(filepath, num):
    str = linecache.getline(filepath, num).strip()
    return str

def getConfig(appdata, webdata, maildata):
    config_file = "/root/config.ini"
    config_data = ConfigParser()
    config_data.read(config_file)

    # print(config_data.sections())
    appdata.Intervals = config_data['APPConfig']['Intervals']
    appdata.SendMAX = config_data['APPConfig']['SendMAX']
    appdata.ServerIP = config_data['APPConfig']['ServerIP']
    appdata.SSConfigPath = config_data['APPConfig']['SSConfigPath']
    with open(appdata.SSConfigPath, 'r') as f: # 读json
        data = json.load(f)
    appdata.ServerPort = str(data["server_port"])

    webdata.InternalURL = config_data['WebConfig']['InternalURL']
    webdata.ForeignURL = config_data['WebConfig']['ForeignURL']
    webdata.Referer = config_data['WebConfig']['Referer']  # 端口在第三行

    maildata.SMTPHost = config_data['MailConfig']['SMTPHost']
    maildata.FromMail = config_data['MailConfig']['FromMail']
    maildata.Password = config_data['MailConfig']['Password']
    maildata.ToMail = config_data['MailConfig']['ToMail']
    maildata.Subject = config_data['MailConfig']['Subject']
    maildata.Content = config_data['MailConfig']['Content']


def getContent(appdata, webdata):
    InternalRequest = Request(webdata.InternalURL + appdata.ServerIP + "/" + appdata.ServerPort)
    InternalRequest.add_header("referer", webdata.Referer)
    print()
    try:
        InternalContent = urlopen(InternalRequest).read().decode("utf-8")
    except Exception as e:
        print("https://www.toolsdaquan.com/ipcheck/ fail.")
        print("----------")
        print(e)
        print("----------")
        return -1
    StrList = InternalContent.split('"')
    webdata.InternalICMP = StrList[3]
    webdata.InternalTCP = StrList[7]

    ForeignRequest = Request(webdata.ForeignURL + appdata.ServerIP + "/" + appdata.ServerPort)
    ForeignRequest.add_header("referer", webdata.Referer)
    ForeignContent = urlopen(ForeignRequest).read().decode("utf-8")
    StrList = ForeignContent.split('"')
    webdata.ForeignICMP = StrList[7]
    webdata.ForeignTCP = StrList[3]

    return 1

def sendEmail(maildata):
    email_client = SMTP(maildata.SMTPHost)
    email_client.login(maildata.FromMail, maildata.Password)
    # email_client.set_debuglevel(1)

    # create msg
    msg = MIMEText(maildata.Content, 'plain', 'utf-8')
    msg['Subject'] = Header(maildata.Subject, 'utf-8')
    msg['From'] = "VPS Notice" + "<" + maildata.FromMail + ">"
    ToList = maildata.ToMail.split(',')
    ToNum = len(ToList)
    for i in range(0, ToNum):
        msg['To'] = ToList[i] + "<" + ToList[i] + ">"
        try:
            email_client.sendmail(maildata.FromMail, ToList[i], msg.as_string())
            print("Email: " + maildata.FromMail + "---->" + ToList[i] + " ok.")
        except Exception as e:
            print("Email: " + maildata.FromMail + "---->" + ToList[i] + " fail.")
            print("----------")
            print(e)
            print("----------")
    email_client.quit()


def changePort(appdata):
    with open(appdata.SSConfigPath, 'r') as f: # 读json
        data = json.load(f)

    appdata.ServerNewPort = random.randint(9000, 65535)
    data["server_port"] = appdata.ServerNewPort

    with open(appdata.SSConfigPath, 'w') as f: # 写json
        json.dump(data, f)

    os.system("/etc/init.d/shadowsocks-python start")  # 重启SS

if __name__ == "__main__":

    appdata = APPData()
    webdata = WebData()
    maildata = MailData()
    getConfig(appdata, webdata, maildata)

    os.system("timedatectl set-timezone Asia/Shanghai")  # 修改时区
    os.system("ntpdate ntp1.aliyun.com")  # 同步时间
    localtime = time.asctime(time.localtime(time.time()))
    print(localtime + " - SSPort start!")
    SendCount = 0

    while (int(appdata.SendMAX) > SendCount):
        getConfig(appdata, webdata, maildata)
        if getContent(appdata, webdata) > 0:
            if (webdata.InternalICMP == "fail"):
                sleep(10)
                getContent(appdata, webdata)
                if (webdata.InternalICMP == "fail"):
                    # ip被封
                    maildata.Subject = "VPS IP Error"
                    maildata.Content = appdata.ServerIP + ": An IP error was detected.\n\rPlease contact the administrator to resolve it."
                    localtime = time.asctime(time.localtime(time.time()))
                    print(localtime + " - " + maildata.Subject + ":" + maildata.Content)
                    sendEmail(maildata)
                    SendCount = SendCount + 1
            elif (webdata.InternalTCP == "fail"):
                sleep(10)
                getContent(appdata, webdata)
                if (webdata.InternalTCP == "fail"):
                    # 端口被封
                    maildata.Subject = "VPS Port Error"
                    changePort(appdata)
                    maildata.Content = appdata.ServerIP + ": A port error was detected.\n\rThe new port is:" + str(appdata.ServerNewPort) + "."
                    localtime = time.asctime(time.localtime(time.time()))
                    print(localtime + " - " + maildata.Subject + ":" + maildata.Content)
                    sendEmail(maildata)
                    SendCount = SendCount + 1
            else:
                sleep(int(appdata.Intervals) * 60)
                localtime = time.asctime(time.localtime(time.time()))
                print(localtime + " - SSPort OK!")
        else:
            sleep(10 * 60)
            localtime = time.asctime(time.localtime(time.time()))
            print(localtime + " - SSPort Retry!")
    print(localtime + " - SSPort END!")
