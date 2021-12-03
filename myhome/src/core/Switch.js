// export default function handleIluminacaoInterna(event) {
//     setIluminacaoInterna(event.target.checked);
//     alert(iluminacaoInterna);
// };

// export default function handleIluminacaoGaragem(event) {
//     setIluminacaoGaragem(event.target.checked);
//     alert(iluminacaoGaragem);
// };

// export default function handleIluminacaoJardim(event) {
//     setIluminacaoJardim(event.target.checked);
//     alert(iluminacaoJardim);
// };

// export default function handleSirene(event) {
//     setSirene(event.target.checked);
//     alert(sirene);
// };

// export default function handleSensorPortaJanela(event) {
//     setSensorPortaJanela(event.target.checked);
//     alert(sensorPortaJanela);
// };

// export default function handleArCondicionado(event) {
//     setArCondicionado(event.target.checked);
//     alert(arCondicionado);
// };
import Switch from '@mui/material/Switch';
import { styled } from '@mui/material/styles';

const AntSwitch = styled(Switch)(({ theme }) => ({
    width: 34,
    height: 20,
    padding: 0,
    display: 'flex',
    '&:active': {
      '& .MuiSwitch-thumb': {
        width: 15,
      },
      '& .MuiSwitch-switchBase.Mui-checked': {
        transform: 'translateX(9px)',
      },
    },
    '& .MuiSwitch-switchBase': {
      padding: 2,
      '&.Mui-checked': {
        transform: 'translateX(12px)',
        color: '#fff',
        '& + .MuiSwitch-track': {
          opacity: 1,
          backgroundColor: theme.palette.mode === 'dark' ? '#177ddc' : '#1890ff',
        },
      },
    },
    '& .MuiSwitch-thumb': {
      boxShadow: '0 2px 4px 0 rgb(0 35 11 / 20%)',
      width: 18,
      height: 16,
      borderRadius: 6,
      transition: theme.transitions.create(['width'], {
        duration: 200,
      }),
    },
    '& .MuiSwitch-track': {
      borderRadius: 16 / 2,
      opacity: 1,
      backgroundColor:
        theme.palette.mode === 'dark' ? 'rgba(255,255,255,.35)' : 'rgba(0,0,0,.25)',
      boxSizing: 'border-box',
    },
  }));

  export default AntSwitch