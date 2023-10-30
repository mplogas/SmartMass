# MQTT topic considerations

## what's needed?

- value
- heartbeat
- calibration result
- configuration cmd
- tare cmd
- write_tag cmd
- calibration cmd

### Status (value)

`BASETOPIC/status/clientid`
`
```json
{
    "device_id": "clientid",
    "spool_id": "guid",
    "value": 100,  
}
```

### Heartbeat

`BASETOPIC/heartbeat/clientid`
`
```json
{
    "device_id": "clientid",
    "status": "ok" 
}
```

### Commands

`BASETOPIC/cmd/clientid/`

```json
{
    "action": "tare"
}
```

```json
{
    "action": "calibrate"
}
```

```json
{
    "action": "configure",
    "scale": {
        "update_interval": 1000,
        "sampling_size": 1,
        "calibration": 981,
        "known_weight": 100
    },
    "display": {
        "display_timeout": 60000
    }
}
```

```json
{
    "action": "write-tag",
    "tag": {
        "spool_id": "75dd14f4-730d-4c52-bc07-fc79164f6c47",
        "spool_weight": 135,
        "material": "PLA",
        "color": "#ff0000",
        "manufacturer": "Sunlu",
        "spool_name": "Sunlu PETG red",
        "timestamp": 1697660662

    }
}
```

###  Command responses

`BASETOPIC/response/clientid/`

```json
{
    "device_id": "client_id"
    "action": "calibrate",
    "result": 981 
}
```