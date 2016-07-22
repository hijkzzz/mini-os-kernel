[global switch_to]

switch_to:
    ; 保存现场
    mov eax, [esp + 4]

    mov [eax + 0],  esp
    mov [eax + 4],  ebp
    mov [eax + 8],  ebx
    mov [eax + 12], esi
    mov [eax + 16], edi
    ; 保存标志寄存器
    pushf
    pop ecx
    mov [eax + 20], ecx

    ; 加载新环境
    mov eax, [esp + 8]

    mov esp, [eax + 0]
    mov ebp, [eax + 4]
    mov ebx, [eax + 8]
    mov esi, [eax + 12]
    mov edi, [eax + 16]
    mov eax, [eax + 20]
    push eax
    popf

    ret
