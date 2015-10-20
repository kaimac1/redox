Configuration of the module is simple. Max retransmission count is set to 15 and delays between each retranmission is set to 1000 us. Also the CRC is enabled with 1byte length. Data rate is set to 1Mbps.

Via the config function, you can chose the channel and the payload length. Max payload length is 32. After the config function the module automatically goes to RX mode. Payload length and the RF channel values have to be consistent among the devices.

	/* initializes hardware pins */
	nrf24_init();
	
	/* RF channel: #2 , payload length: 4 */
	nrf24_config(2,4)





After the transmission end, optionally you can make analysis on the last transmission attempt.

	uint8_t temp;
	
	/* Make analysis on last tranmission attempt */
	temp = nrf24_lastMessageStatus();

	if(temp == NRF24_TRANSMISSON_OK)
	{					
		xprintf("Tranmission went OK\r\n");
	}
	else if(temp == NRF24_MESSAGE_LOST)
	{					
		xprintf("Message is lost ...\r\n");	
	}
      	
    /* Retranmission count indicates the tranmission quality */
    temp = nrf24_retransmissionCount();
    xprintf("Retranmission count: %d\r\n",temp);
    
After the tranmission finishes, nrf24L01+ module stays in Standby mode. You can manually go back to RX mode:

	/* Go back to RX mode ... */
    nrf24_powerUpRx();
 
Or you can power down the module to lower the current consumption.

    /* Power down the module */
    nrf24_powerDown();	
    
### Receive

This library doesn't use the IRQ pin of the nrf24L01+ , therefore you need to poll the `nrf24_dataReady()` function periodically. Otherwise you might miss some packets. 

Also, you need to be in RX mode in order to be able to receive messages.

`nrf24_dataReady()` function returns non-zero only if a valid payload is awaiting in the RX fifo. `nrf24_getData(uint8_t* buf)` function copies the received message into the given buffer. 

	uint8_t data_array[4];

	if(nrf24_dataReady())
	{
		nrf24_getData(data_array);		
	}



- https://github.com/aaronds/arduino-nrf24l01
- http://www.tinkerer.eu/AVRLib/nRF24L01
