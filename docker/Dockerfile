FROM python:3.8-alpine

RUN pip3 install --upgrade pip
RUN apk add gcc build-base

RUN mkdir /opt/data
WORKDIR /opt/data
COPY ../requirements.txt /opt/data/

RUN pip3 install -r /opt/data/requirements.txt

COPY .. /opt/data
