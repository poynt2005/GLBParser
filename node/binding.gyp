{
    "targets": [
        {
            "target_name": "GLBParser",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": ["NodeBinding.cc"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")",
                "<!@(node -p \"require('./path').GLTFSDKInclude\")"
            ],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'libraries': ["<!@(node -p \"require('./path.js').GLBParserLib\")", "<!@(node -p \"require('./path.js').GLBParserStreamLib\")", "<!@(node -p \"require('./path.js').GLBSDKLib\")", "rpcrt4.lib"],
            'msvs_settings': {
                'VCCLCompilerTool': {
                    'AdditionalOptions': ['-std:c++17', ]
                },
            },
            'configurations': {
                'Debug': {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'RuntimeLibrary': 3
                        },
                    },
                },
                'Release': {
                    'msvs_settings': {
                        'VCCLCompilerTool': {
                            'RuntimeLibrary': 2
                        },
                    },
                }
            }
        }
    ]
}
