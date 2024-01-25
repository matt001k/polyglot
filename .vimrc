" Set arm debugging
packadd termdebug
let g:termdebugger = $AHRIMAN_ARM_TOOLCHAIN . "/arm-none-eabi-gdb"
let g:termdebug_popup = 0
let g:termdebug_wide = 75

function! StartDebugApp()
    :silent !make flash
    :silent !screen -dm $AHRIMAN_GDB_SERVER -singlerun -nogui -if swd -port 50000 -swoport 50001 -telnetport 50002 -device EFM32TG11B520F128
    :redraw!
    Termdebug -x ./.vim/app_debug
endfunction

function! StartDebugBoot()
    :silent !screen -dm $AHRIMAN_GDB_SERVER -singlerun -nogui -if swd -port 50000 -swoport 50001 -telnetport 50002 -device EFM32TG11B520F128
    :redraw!
    Termdebug -x ./.vim/boot_debug
endfunction

" Map ESC to exit terminal mode
nnoremap <leader>da :call StartDebugApp()<CR>
nnoremap <leader>db :call StartDebugBoot()<CR>
tnoremap <Esc> <C-\><C-n>
nnoremap <silent> <leader>b :Break<CR>
nnoremap <silent> <leader>bc :Clear<CR>
nnoremap <silent> <leader>c :Continue<CR>
nnoremap <silent> <leader>s :Stop<CR>
nnoremap <silent> <leader>i :Step<CR>
nnoremap <silent> <leader>o :Over<CR>
nnoremap <silent> <leader>p :Finish<CR>
nnoremap <silent> <leader>e :Evaluate<CR>

