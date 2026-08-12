{
  "graph": {
    "cells": [
      {
        "position": {
          "x": 0,
          "y": 0
        },
        "size": {
          "height": 10,
          "width": 10
        },
        "type": "Statechart",
        "id": "10000000-0000-0000-0000-000000000000",
        "attrs": {
          "name": {
            "text": "test Export"
          },
          "specification": {
            "text": "@EventDriven\n\ninternal:\n    var setpoint_preview : integer = 0\n    var setpoint_angle : integer = 0\n    var setpoint_deg : real = 0.0\n    var tita_deg : real = 0.0\n    var pid_enabled : boolean = false\n\ninterface:\n    in event EV_MEN_ENT_ACTIVE\n    in event EV_MEN_NEX_ACTIVE\n    in event EV_MEN_ESC_ACTIVE\n"
          }
        },
        "z": 1
      },
      {
        "position": {
          "x": -309,
          "y": -330
        },
        "size": {
          "height": 81,
          "width": 226
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_MAIN_CONFIG",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000001",
        "z": 88
      },
      {
        "position": {
          "x": -20,
          "y": -94
        },
        "size": {
          "height": 60,
          "width": 130
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_MAIN_ACTION",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000002",
        "z": 101
      },
      {
        "position": {
          "x": 230,
          "y": -107
        },
        "size": {
          "height": 67,
          "width": 148
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_ACTION_CONTROL",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000009",
        "z": 127
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000002"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000009"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE"
              }
            },
            "position": {
              "distance": 0.5132848906935307,
              "offset": -10,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000004",
        "z": 128,
        "vertices": []
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000009"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000002"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.5093567608392726,
              "offset": -13,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "3"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000014",
        "z": 128,
        "vertices": []
      },
      {
        "position": {
          "x": -639,
          "y": -114
        },
        "size": {
          "height": 60,
          "width": 262
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_CONFIG_SETPOINT",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000003",
        "z": 163
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000003",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "99.656%",
              "dy": "80.769%",
              "rotate": true
            }
          },
          "priority": true
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000001"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.7723828635528972,
              "offset": -52,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "3"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000007",
        "z": 202,
        "vertices": [
          {
            "x": -300,
            "y": -115
          }
        ]
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000001"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000003"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE"
              }
            },
            "position": {
              "distance": 0.15288056395912744,
              "offset": 11.85434592457326,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000002",
        "z": 203
      },
      {
        "position": {
          "x": -216,
          "y": -414
        },
        "size": {
          "height": 18,
          "width": 18
        },
        "type": "Entry",
        "entryKind": "Initial",
        "attrs": {},
        "id": "60000000-0000-0000-0000-000000000000",
        "z": 211,
        "embeds": [
          "60000000-0000-0000-0000-000000000001"
        ]
      },
      {
        "type": "NodeLabel",
        "label": true,
        "size": {
          "width": 15,
          "height": 15
        },
        "position": {
          "x": -216,
          "y": -399
        },
        "attrs": {
          "label": {
            "refX": "50%",
            "textAnchor": "middle",
            "refY": "50%",
            "textVerticalAnchor": "middle"
          }
        },
        "id": "60000000-0000-0000-0000-000000000001",
        "z": 212,
        "parent": "60000000-0000-0000-0000-000000000000"
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "60000000-0000-0000-0000-000000000000"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000001"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {},
            "position": {}
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000000",
        "z": 213
      },
      {
        "position": {
          "x": -680,
          "y": 219
        },
        "size": {
          "height": 60,
          "width": 171
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_SETPOINT_CONFIRM",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000006",
        "z": 236
      },
      {
        "position": {
          "x": 260,
          "y": 231
        },
        "size": {
          "height": 60,
          "width": 150
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_ANGLE_REPORT",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-00000000000c",
        "z": 248
      },
      {
        "position": {
          "x": 559,
          "y": -99
        },
        "size": {
          "height": 60,
          "width": 200
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_PID_CONTROL",
            "fontSize": 12
          },
          "specification": {
            "text": "entry / pid_enabled = true\nexit / pid_enabled = false"
          }
        },
        "id": "50000000-0000-0000-0000-00000000000b",
        "z": 253
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000009"
        },
        "target": {
          "id": "50000000-0000-0000-0000-00000000000b"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE"
              }
            },
            "position": {
              "distance": 0.48091133004926107,
              "offset": 13,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000013",
        "z": 254,
        "vertices": [
          {
            "x": 532,
            "y": -51
          }
        ]
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-00000000000b"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000009"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.5091955626539408,
              "offset": 11,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000018",
        "z": 254
      },
      {
        "position": {
          "x": -490,
          "y": 54
        },
        "size": {
          "height": 60,
          "width": 147
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_CONFIG_REPORT",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000004",
        "z": 255
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000003"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000004",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "65.306%",
              "dy": "1.667%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_NEX_ACTIVE"
              }
            },
            "position": {
              "distance": 0.5334843105740017,
              "offset": -56,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000005",
        "z": 256,
        "vertices": []
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000004"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000001"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.4958003535605434,
              "offset": 49,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "3"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-00000000000a",
        "z": 256
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000004"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000003"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_NEX_ACTIVE"
              }
            },
            "position": {
              "distance": 0.7654153461809512,
              "offset": -50,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000008",
        "z": 256
      },
      {
        "position": {
          "x": -489,
          "y": 185
        },
        "size": {
          "height": 61,
          "width": 143
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_REPORT_MODE",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000008",
        "z": 257
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000004"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000008",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "70%",
              "dy": "1.667%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE"
              }
            },
            "position": {
              "distance": 0.5361990950226244,
              "offset": -33,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000009",
        "z": 258
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000008"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000004",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "29%",
              "dy": "96.667%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.6951248813682879,
              "offset": -50.984003906249995,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000011",
        "z": 258
      },
      {
        "position": {
          "x": -688,
          "y": 50
        },
        "size": {
          "height": 60,
          "width": 157
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_SETPOINT_VALUE",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000005",
        "z": 267
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000006"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000005",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "75%",
              "dy": "96.667%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.40058548953555045,
              "offset": 50,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-00000000000e",
        "z": 268,
        "vertices": []
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000003",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "4.889%",
              "dy": "93.333%",
              "rotate": true
            }
          },
          "priority": true
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000005"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE"
              }
            },
            "position": {
              "distance": 0.6112260540712227,
              "offset": 68,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000006",
        "z": 268
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000005"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000003",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "12.027%",
              "dy": "91.026%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.35969709891539353,
              "offset": 51.00000976562501,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "id": "6417f084-ad84-4e33-8b25-a3a5bf5f882d",
        "z": 268,
        "router": {
          "name": "orthogonal"
        },
        "vertices": []
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000005"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000006"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE / setpoint_angle = setpoint_preview"
              }
            },
            "position": {
              "distance": 0.14526422674005682,
              "offset": 157,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-00000000000b",
        "z": 268
      },
      {
        "position": {
          "x": -1028,
          "y": 221
        },
        "size": {
          "height": 60,
          "width": 149
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_SETTINGS_SAVED",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-000000000007",
        "z": 269
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000006"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000007"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE / setpoint_deg = setpoint_angle"
              }
            },
            "position": {
              "distance": 0.40549667105490333,
              "offset": -46,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-00000000000d",
        "z": 270
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000007"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000003",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "1.031%",
              "dy": "30.769%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.28801370264601045,
              "offset": -54,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000010",
        "z": 270,
        "vertices": [
          {
            "x": -969,
            "y": -63
          }
        ]
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000007"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000003",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "0.344%",
              "dy": "78.205%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE"
              }
            },
            "position": {
              "distance": 0.32808730585984575,
              "offset": 53,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-00000000000f",
        "z": 270,
        "vertices": [
          {
            "x": -938,
            "y": 103
          },
          {
            "x": -788,
            "y": -67.08
          }
        ]
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000002"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000001",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "97.788%",
              "dy": "64.198%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_NEX_ACTIVE"
              }
            },
            "position": {
              "distance": 0.8142050763944495,
              "offset": -16.787445068359375,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000003",
        "z": 273,
        "vertices": [
          {
            "x": 20,
            "y": -143
          }
        ]
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000001",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "96.46%",
              "dy": "16.049%",
              "rotate": true
            }
          },
          "priority": true
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000002"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_NEX_ACTIVE"
              }
            },
            "position": {
              "distance": 0.2407760591458211,
              "offset": -8.795665620012372,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000001",
        "z": 274
      },
      {
        "position": {
          "x": 247,
          "y": 76
        },
        "size": {
          "height": 60,
          "width": 136
        },
        "type": "State",
        "attrs": {
          "name": {
            "text": "ST_MEN_ACTION_REPORT",
            "fontSize": 12
          }
        },
        "id": "50000000-0000-0000-0000-00000000000a",
        "z": 275
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-00000000000c",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "30.667%",
              "dy": "0%",
              "rotate": true
            }
          },
          "priority": true
        },
        "target": {
          "id": "50000000-0000-0000-0000-00000000000a"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.6022194310238487,
              "offset": -65,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000019",
        "z": 276
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-000000000009"
        },
        "target": {
          "id": "50000000-0000-0000-0000-00000000000a"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_NEX_ACTIVE"
              }
            },
            "position": {
              "distance": 0.6208053691275168,
              "offset": -57,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000012",
        "z": 276,
        "vertices": []
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-00000000000a"
        },
        "target": {
          "id": "50000000-0000-0000-0000-00000000000c",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "56.667%",
              "dy": "3.333%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ENT_ACTIVE"
              }
            },
            "position": {
              "distance": 0.6193989000822369,
              "offset": -52,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "2"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000016",
        "z": 276,
        "vertices": []
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-00000000000a"
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000002"
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_ESC_ACTIVE"
              }
            },
            "position": {
              "distance": 0.4810386077416461,
              "offset": -18.83511159400255,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "3"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000017",
        "z": 276,
        "vertices": [
          {
            "x": 195,
            "y": 100
          }
        ]
      },
      {
        "type": "Transition",
        "attrs": {},
        "source": {
          "id": "50000000-0000-0000-0000-00000000000a",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "23.529%",
              "dy": "1.667%",
              "rotate": true
            }
          },
          "priority": true
        },
        "target": {
          "id": "50000000-0000-0000-0000-000000000009",
          "anchor": {
            "name": "topLeft",
            "args": {
              "dx": "33.108%",
              "dy": "95.522%",
              "rotate": true
            }
          },
          "priority": true
        },
        "connector": {
          "name": "rounded"
        },
        "labels": [
          {
            "attrs": {
              "text": {
                "text": "EV_MEN_NEX_ACTIVE"
              }
            },
            "position": {
              "distance": 0.2332071483644665,
              "offset": -61,
              "angle": 0
            }
          },
          {
            "attrs": {
              "label": {
                "text": "1"
              }
            }
          },
          {
            "attrs": {}
          },
          {
            "attrs": {}
          }
        ],
        "router": {
          "name": "orthogonal",
          "args": {
            "padding": 8
          }
        },
        "id": "70000000-0000-0000-0000-000000000015",
        "z": 277,
        "vertices": [
          {
            "x": 279,
            "y": 25
          }
        ]
      }
    ]
  },
  "genModel": {
    "generator": {
      "type": "create::c",
      "features": {
        "Outlet": {
          "targetProject": "",
          "targetFolder": "",
          "libraryTargetFolder": "",
          "skipLibraryFiles": "",
          "apiTargetFolder": ""
        },
        "LicenseHeader": {
          "licenseText": ""
        },
        "FunctionInlining": {
          "inlineReactions": false,
          "inlineEntryActions": false,
          "inlineExitActions": false,
          "inlineEnterSequences": false,
          "inlineExitSequences": false,
          "inlineChoices": false,
          "inlineEnterRegion": false,
          "inlineExitRegion": false,
          "inlineEntries": false
        },
        "OutEventAPI": {
          "observables": false,
          "getters": false
        },
        "IdentifierSettings": {
          "moduleName": "Test",
          "statemachinePrefix": "test",
          "separator": "_",
          "headerFilenameExtension": "h",
          "sourceFilenameExtension": "c"
        },
        "Tracing": {
          "enterState": false,
          "exitState": false,
          "generic": false
        },
        "Includes": {
          "useRelativePaths": false,
          "generateAllSpecifiedIncludes": false
        },
        "GeneratorOptions": {
          "userAllocatedQueue": false,
          "metaSource": false
        },
        "GeneralFeatures": {
          "timerService": false,
          "timerServiceTimeType": ""
        },
        "Debug": {
          "dumpSexec": false
        }
      }
    }
  }
}