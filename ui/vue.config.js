module.exports = {
  filenameHashing: false,

  pluginOptions: {
    quasar: {
      treeShake: true
    }
  },

  transpileDependencies: [
    /[\\\/]node_modules[\\\/]quasar[\\\/]/
  ],

  chainWebpack: config => {
    config.plugins.delete('preload');
    config.plugins.delete('prefetch');
    config.optimization.delete('splitChunks');

    config.plugin('html')
      .tap(args => {
        args[0].inject = process.env.NODE_ENV !== 'production';
        args[0].minify = false;
        args[0].templateParameters = {
          CDN: process.env.CDN_URL || '',
          NODE_ENV: process.env.NODE_ENV,
        };
        return args;
      });
  }
};
