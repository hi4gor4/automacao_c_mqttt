import React, { useEffect, useState } from 'react';
import AppBar from '@mui/material/AppBar';
import Toolbar from '@mui/material/Toolbar';
import Typography from '@mui/material/Typography';
import Grid from '@mui/material/Grid';
import Button from '@mui/material/Button';
import mqtt from 'mqtt';
import AntSwitch from "../core/Switch"
import Paper from '@mui/material/Paper';
import TextField from '@mui/material/TextField';
import Stack from '@mui/material/Stack';
import ModalContent from "./components/Modal"


const Topicos = {
  "hiago23rangel@gmail.com/alarm1": "alarm1",
  "hiago23rangel@gmail.com/alarm2": "ativar",
  "hiago23rangel@gmail.com/luz1": "luz1",
  "hiago23rangel@gmail.com/luz2": "luz2",
  "hiago23rangel@gmail.com/alarm1": "alarm1",
  "hiago23rangel@gmail.com/alarm2": "alarm2",
  "hiago23rangel@gmail.com/temp": "temp"
  //"hiago23rangel@gmail.com/pbl2"
}

export default function Content({ title }) {
  const [data, setData] = useState([])
  const [client, setClient] = useState(null)
  const [connectStatus, setConnectStatus] = useState('Disconnect');



  const [open, setOpen] = React.useState(false);
  const handleClose = () => setOpen(false);

  const [Variaveis, setVariaveis] = useState({
    alarme: false,
    luz1: true,
    min: "17",
    max: "25",
    temperatura: "20"
  })

  const [Exibir, setExibir] = useState({
    luz1: false,
    min: "",
    max: "",
    temperatura: "20",
    alarm2: "false"
  })

  const set = (newValue) => {
    setVariaveis((form) => ({ ...form, ...newValue }));
  };

  const setMostrar = (newValue) => {
    console.log(newValue)
    setExibir((form) => ({ ...form, ...newValue }));
  };


  const mqttConnect = () => {
    setConnectStatus('Connecting');
    const options = {
      keepalive: 30,
      port: 8883,
      protocolId: 'MQTT',
      protocolVersion: 4,
      clean: true,
      reconnectPeriod: 1000,
      connectTimeout: 30 * 1000,
      will: {
        topic: 'WillMsg',
        payload: 'Connection Closed abnormally..!',
        qos: 0,
        retain: false
      },
      rejectUnauthorized: false
    };
    options.clientId = "web"
    options.username = "hiago23rangel@gmail.com"
    options.password = "2314"
    setClient(mqtt.connect('mqtt://maqiatto.com', options));
  };

  const mqttSubscribe = () => {
    if (client) {

      client.subscribe([
        "hiago23rangel@gmail.com/alarm1",
        "hiago23rangel@gmail.com/luz1",
        "hiago23rangel@gmail.com/luz2",
        "hiago23rangel@gmail.com/max",
        "hiago23rangel@gmail.com/min",
        "hiago23rangel@gmail.com/alarm2",
        "hiago23rangel@gmail.com/temp",
        "hiago23rangel@gmail.com/pbl2"], { qos: 1 }, (error) => {
          if (error) {
            console.log('Subscribe to topics error', error)
            return
          }
        });
    }
  };

 
  const mqttPublish = (context) => {
    if (client) {
      console.log("Publicando")
      const { topic, qos, payload } = context;
      client.publish(topic, payload, { qos }, error => {
        if (error) {
          console.log('Publish error: ', error);
        }
      });
    }
  }

  function Armin() {
    mqttPublish({"topic": "hiago23rangel@gmail.com/min", "qos": 1, "payload": Exibir.min})
  }

  function Armax() {
    mqttPublish({"topic": "hiago23rangel@gmail.com/max","qos": 1,"payload": Exibir.max})
  }

  function Temperatura() {
    mqttPublish({"topic": "hiago23rangel@gmail.com/temp", "qos": 1, "payload": Exibir.temperatura})
  }

  function botao(event, topic, field){
    setExibir({[field]: event.target.checked})
    mqttPublish({ "topic": topic,"qos": 1,"payload": event.target.checked === true ? "1" : "0"})
  }

  useEffect(() => {
    mqttConnect()
  }, []);

  useEffect(() => {
    if (client) {
      client.on('connect', () => {
        setConnectStatus('Connected');
      });
      client.on('error', (err) => {
        console.error('Connection error: ', err);
        client.end();
      });
      client.on('reconnect', () => {
        setConnectStatus('Reconnecting');
      });
      client.on('message', (topic, message) => {
        if (message == "1") message = true
        else if (message == "0") message = false
        setMostrar({[Topicos[topic]]: message })
      });
    }
  }, [client]);

  useEffect(() => {
    if (connectStatus == "Connected") {
      mqttSubscribe();
    }
  }, [connectStatus]);

  return (
    <>
      <Paper style={{ marginTop: '20px' }} sx={{ maxWidth: 936, margin: 'auto', overflow: 'hidden' }}>
        <AppBar position="static" color="default"elevation={0} sx={{borderBottom: '1px solid rgba(0, 0, 0, 0.12)'}}>
          <Toolbar>
            Minha Casa
          </Toolbar>
        </AppBar>
        <Grid container spacing={2} alignItems="center">
          <Grid item xs={12}>
            Alarme:
            <Stack direction="row" spacing={1} alignItems="center">
              <Typography>Desativado</Typography>
              <AntSwitch checked={Variaveis.alarme} inputProps={{ 'aria-label': 'ant design' }}/>
              <Typography>Ativo</Typography>
            </Stack>
            <Button onClick={e => {botao(e, "hiago23rangel@gmail.com/alarm1", "alarm1")}} variant="contained" color="primary" >
                Ligar/Desligar
              </Button>
          </Grid>

          <Grid item xs={12}>
            Intruso:
            <Stack direction="row" spacing={1} alignItems="center">
              <Typography>Desativado</Typography>
              <AntSwitch checked={Exibir.alarm2} inputProps={{ 'aria-label': 'ant design' }}/>
              <Typography>Ativo</Typography>
            </Stack>
          </Grid>

          <Grid item xs={12}>
            Luz 1:
            <Stack direction="row" spacing={1} alignItems="center">
              <Typography>Desativado</Typography>
              <AntSwitch checked={Variaveis.luz1} inputProps={{ 'aria-label': 'ant design' }}/>
              <Typography>Ativo</Typography>
            </Stack>
            <Stack direction="collun" spacing={1} alignItems="center">
              <Button onClick={e => {botao(e, "hiago23rangel@gmail.com/luz1", "luz1")}} variant="contained" color="primary" >
                Ligar/Desligar
              </Button>
            </Stack>
          </Grid>

          <Grid item xs={12}>
            Luz 2:
            <Stack direction="row" spacing={1} alignItems="center">
              <Typography>Desativado</Typography>
              <AntSwitch checked={Variaveis.luz1} inputProps={{ 'aria-label': 'ant design' }}/>
              <Typography>Ativo</Typography>
            </Stack>
            <Stack direction="row" spacing={1} alignItems="center">
              <Button onClick={e => {botao(e, "hiago23rangel@gmail.com/luz2", "luz2")}} variant="contained" color="primary" >
                Ligar/Desligar
              </Button>
            </Stack>
          </Grid>

          <Grid item xs={6}>
            <Stack style={{ marginTop: '10px' }} direction="row" spacing={1} alignItems="center">
              <TextField label="Temperatura Minima" value={Variaveis.min} onChange={(e) => {Armin(); set({ min: e.target.value })}} inputProps={{ inputMode: 'numeric', pattern: '[0-9]*' }} type="number"/>
            </Stack>
          </Grid>

          <Grid item xs={6}>
            <Stack style={{ marginTop: '10px' }} direction="row" spacing={1} alignItems="center">
              <TextField
                label="Temperatura" value={Variaveis.temperatura} onChange={(e) => {Temperatura(); set({temperatura: e.target.value })}} inputProps={{ inputMode:'numeric', pattern:'[0-9]*'}} type="number"/>
            </Stack>
          </Grid>

          <Grid item xs={6}>
            <Stack style={{ marginTop: '10px' }} direction="row" spacing={1} alignItems="center">
              <TextField label="Temperatura Maxima" value={Variaveis.max} onChange={(e) => {Armax() ; set({max: e.target.value})}} inputProps={{ inputMode: 'numeric', pattern: '[0-9]*'}} type="number"/>
            </Stack>
          </Grid>
        </Grid>
      </Paper>
      <ModalContent open={open} handleClose={handleClose}data={data}/>
    </>
  );
}