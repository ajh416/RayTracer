class Texture {
      public:
        Texture(const char *path);
        ~Texture();
        void Bind(unsigned int slot = 0) const;
        void Unbind() const;
        inline int GetWidth() const { return width; }
        inline int GetHeight() const { return height; }
        inline unsigned int GetRendererID() const { return renderer_id; }

      private:
        unsigned int renderer_id;
        unsigned char *local_buffer;
        int width, height, bpp;
};
