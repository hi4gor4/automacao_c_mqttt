import React, { useEffect, useState } from 'react';
import Box from '@mui/material/Box';
import Typography from '@mui/material/Typography';
import List from '@mui/material/List';
import Modal from '@mui/material/Modal';
import Button from '@mui/material/Button';

const style = {
    position: 'absolute',
    top: '50%',
    left: '50%',
    transform: 'translate(-50%, -50%)',
    width: 650,
    bgcolor: 'background.paper',
    border: '2px solid #000',
    boxShadow: 24,
    p: 4,
};

export default function ModalContent({ open, handleClose, data }) {
    return (
        <>
            <Modal
                open={open}
                onClose={handleClose}
                aria-labelledby="modal-modal-title"
                aria-describedby="modal-modal-description"
            >
                <Box sx={style}>
                    <Typography id="modal-modal-title" variant="h5" component="h2">
                        Historico Alarme
                    </Typography>
                    <Typography id="modal-modal-description" sx={{ mt: 2 }}>
                        {
                            data.map((item) => {
                                var data = item.data.split('T')[0]
                                var time = item.data.split('T')[1]

                                return (
                                    <List>
                                        <Typography
                                            variant="h8"
                                        >
                                            Nome: {item.nome} / Estado: {item.estado === 1 ? 'ativo' : 'desativado'} / Data: {data.split('-')[2]}-{data.split('-')[1]}-{data.split('-')[0]} / Horario: {time}
                                        </Typography>
                                    </List>
                                );
                            }).reverse()
                        }
                    </Typography>

                    <Button style={{ marginTop: '20px' }} onClick={handleClose} variant="contained" color="error" sx={{ mr: 1 }}>
                        Fechar
                    </Button>
                </Box>
            </Modal>
        </>
    );
}