FROM python:3.8-alpine

RUN pip3 install --upgrade pip

RUN mkdir /opt/data
WORKDIR /opt/data
COPY . /opt/data
